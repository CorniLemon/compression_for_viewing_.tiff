#pragma once
#include <iostream>
#include <Windows.h>
#include<vector>
using namespace std;

class TiffFile {
private:
    vector<WORD> R;
    vector<WORD> G;
    vector<WORD> B;
    int lenghtOfLine;
public:
    size_t height = 0;//высота
    size_t width = 0;//ширина
    
    vector<uint32_t> startsStrips;//массив уазателей на начало полос
    int countOfStripes = 0;
    int stringsInStripe = 0;
    int countOfChanals = 0;

    int maxR = 65535;
    int minR = 0;
    int maxG = 65535;
    int minG = 0;
    int maxB = 65535;
    int minB = 0;

    TiffFile(FILE* f) {//читает header
        #pragma pack(push, 1)
        struct HeadOfTiff {
            uint16_t IIorMM;
            uint16_t isItTiff;
            uint32_t offset;
        } head;
        #pragma pack(pop)

        uint16_t count;//количество тегов

        #pragma pack(push, 1)
        struct IFD {//теги
            uint16_t tag;
            uint16_t type;
            uint32_t countOfType;
            uint32_t valueOffset;
            void printIfd() {
                cout << "tag = " << tag << endl;
                cout << "type = " << type << endl;//почему 120 когда наверное 12?
                cout << "countOfType = " << countOfType << endl;
                cout << "valueOffset = " << valueOffset << endl;
            }
        };
        #pragma pack(pop)

        int bitOnPix = 0;//количество бит на канал
        uint32_t startOfPtrToStripes = 0;

        fread(&head, sizeof(head), 1, f);//чтение заголовка файла

        fseek(f, head.offset, SEEK_SET);//переход и чтение вектора тегов
        fread(&count, 2, 1, f);
        vector<IFD> tags(count);//вектор тегов
        fread(tags.data(), sizeof(IFD), count, f);//чтение тегов

        for (int i = 0; i < count; ++i)//поиск необходимых тегов
        {
            switch (tags[i].tag)
            {
            case 256:
                width = tags[i].valueOffset;
                break;
            case 257:
                height = tags[i].valueOffset;
                break;
            case 258:
                fseek(f, tags[i].valueOffset, SEEK_SET);
                fread(&bitOnPix, 2, 1, f);//2 соответствует типу
                if (bitOnPix != 16)
                    throw exception("канал кодируется НЕ 16 битами!");
                break;
            case 259:
                if (tags[i].valueOffset != 1)
                    throw exception("файл сжат!");
                break;
            case 262:
                if (tags[i].valueOffset != 1)
                    throw exception("чёрный кодируется 255, т.е. все цвета инвертированы");
                break;
            case 273:
                countOfStripes = tags[i].countOfType;
                startsStrips.resize(countOfStripes);
                if (countOfStripes == 1) {
                    startsStrips[0] = tags[i].valueOffset;
                }
                else {
                    startOfPtrToStripes = tags[i].valueOffset;
                    fseek(f, startOfPtrToStripes, SEEK_SET);
                    fread(startsStrips.data(), 4, countOfStripes, f);//4 соответствует типу
                }
                break;
            case 277:
                countOfChanals = tags[i].valueOffset;
                if (tags[i].valueOffset != 3)
                    throw exception("в изображении не 3 канала и его не выйдет корректно преобразовать в пнг!");
                break;
            case 278:
                stringsInStripe = tags[i].valueOffset;
                cout << "строк в полосе: " << stringsInStripe << endl;
                break;
            default:
                break;
            }
        }

        if (!height || !width || !bitOnPix||!startOfPtrToStripes||!sizeof(startsStrips)||!stringsInStripe||!countOfChanals) {//проверка наличия некотрых тегов 
            throw exception("нет всех необходимых тегов");
        }

        cout << "количество полос: " << countOfStripes << endl;
        cout << "высота = " << height << endl;
        cout << "ширина = " << width << endl;
        cout << "бит на канал = " << bitOnPix << endl;

        lenghtOfLine = height * width;
        R.resize(lenghtOfLine);
        G.resize(lenghtOfLine);
        B.resize(lenghtOfLine);

        auto print = [&]() {//вывод в [], т.к. эти данные не будут храниться в памяти после закрытия конструктора
            cout << "IIorMM = " << head.IIorMM << endl;
            cout << "isItTiff = " << head.isItTiff << endl;
            cout << "offset = " << head.offset << endl;
            cout << endl;
            cout << "count = " << count << endl;
            cout << endl;
            for (int i = 0; i < count; ++i) {
                tags[i].printIfd();
                cout << endl;
            }
        };

        //print();
    };

    void getBrightness(FILE* f) {
        vector<WORD> line (width * 3);
        
        cout << "проверка связи 1.1\n";

        cout << "...читаем файл для анализа яркости...\n";
        for (int i = 0; i < height; ++i) {//чтение
            getLine(f, i, line.data());
            for (int j = 0; j < width; ++j) {
                R[i * width + j] = line[j * 3];
                G[i * width + j] = line[j * 3 + 1];
                B[i * width + j] = line[j * 3 + 2];
            }
        }

        cout << "проверка связи 1.2\n";

        /*поиск выбросов*/
        int Q1, Q3;
        Q1 = lenghtOfLine / 4;
        Q3 = Q1 * 3;
        //cout << R[0] << " " << R[Q1] << " " << R[Q3] << " " << R[width - 1] << endl;

        /*bubbleSort(R.data(), lenghtOfLine);
        cout << "\nпроверка связи 1.2.1\n";
        bubbleSort(G.data(), lenghtOfLine);
        cout << "\nпроверка связи 1.2.2\n";
        bubbleSort(B.data(), lenghtOfLine);*/

        /*insertionSort(R.data(), lenghtOfLine);
        cout << "\nпроверка связи 1.2.1\n";
        insertionSort(G.data(), lenghtOfLine);
        cout << "\nпроверка связи 1.2.2\n";
        insertionSort(B.data(), lenghtOfLine);*/

        cout << "проверка связи 1.3\n";

        /*внутренние границы*/
        minR = R[Q1] - ((R[Q3] - R[Q1]) * 1.5);
        maxR = R[Q3] + ((R[Q3] - R[Q1]) * 1.5);
        minG = G[Q1] - ((G[Q3] - G[Q1]) * 1.5);
        maxG = G[Q3] + ((G[Q3] - G[Q1]) * 1.5);
        minB = B[Q1] - ((B[Q3] - B[Q1]) * 1.5);
        maxB = B[Q3] + ((B[Q3] - B[Q1]) * 1.5);
        cout << "\nдиапазон R: " << minR << ":" << maxR;
        cout << "\nдиапазон G: " << minG << ":" << maxG;
        cout << "\nдиапазон B: " << minB << ":" << maxB<<"\n";

        cout << "проверка связи 1.4\n";

        /*cout << "\nкоэффициент" << 255 / (maxR - minR) << endl;
        cout << "\nкоэффициент" << 255 / (maxG - minG) << endl;
        cout << "\nкоэффициент" << 255 / (maxB - minB) << endl;*/

        cout << R[0] << " " << R[Q1] << " " << R[Q3] << " " << R[lenghtOfLine - 1] << endl;
        cout << G[0] << " " << G[Q1] << " " << G[Q3] << " " << G[lenghtOfLine - 1] << endl;
        cout << B[0] << " " << B[Q1] << " " << B[Q3] << " " << B[lenghtOfLine - 1] << endl;
        /*cout << "\n всего пикселей: " << lenghtOfLine;
        cout << "\n Q1: " << Q1;
        cout << "\n Q3: " << Q3;*/
    }

    void getLine(FILE* f, size_t i, WORD* line1) {//получает номер строки и читает её. fread вызывается
        /*функция работает только в том случае, если getLine вызывается последовательно, для каждой строки файла*/
        int num = i / stringsInStripe;
        if (!(i % stringsInStripe)) {
            fseek(f, startsStrips[num], SEEK_SET);
        }
        fread(line1, (width * 3 * 2), 1, f);
    }  
};