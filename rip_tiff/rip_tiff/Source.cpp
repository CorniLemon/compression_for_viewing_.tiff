//#include <TIFF.h>//почему не работает то?
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include<vector>
using namespace std;
#include <stdint.h>
#include "TIFF.h"

typedef unsigned __int16 WORD;//2 байта
//typedef unsigned int DWORD;//4 байта
typedef long LONG;//4 байта
typedef unsigned char BYTE;//1 байт

class FileWithDes {
    FILE* f = NULL;
public:
    FileWithDes(const char A[], const char B[]) {
        f = fopen(A, B);
        if (!f) {
            cout << "файл " << A << " не существует или не удалось создать\n";
            throw 1;
        }
        //throw 1;
    };

    FILE* getF() {
        return f;
    };

    ~FileWithDes() {
        fclose(f);
        f = NULL;
    }
};

class Line {
    size_t n = 0;
    BYTE* line = NULL;
public:
    Line(size_t n)
        :n(n), line(new BYTE[n])
    {};

    Line(BYTE* line1)
    {
        line = line1;
    };

    BYTE* data() {
        return line;
    };

    BYTE& operator[] (int i) {
        /*if (i < 0 || i >= n)
            throw 1;*/
        return line[i];
    }

    ~Line() {
        if (line)
        {
            delete[] line;
        }
        line = NULL;
    };
};

int main() {
    setlocale(LC_ALL, "Russian");

    FileWithDes f1("0041_0102_01567_1_01497_03_S_fr.tiff", "rb");//создание потока для чтения tiff
    TiffFile initialFiile(f1.getF());//данные о tiff

    int Add = 1;
    cout << "пожалуйста, введите количество пикселей, которые будут объединены в 1 (сторона квадратной области)\n";
    cin >> Add;

    if ((Add >= initialFiile.WIDTH) || (Add >= initialFiile.HIGHT))//проверка add
    {
        cout << "количество объединяемых пикселей слишком большое (изображение сожмется в картинку 1*1)";
        return 0;
    }

    FileWithDes f2("final.bmp", "wb");
    //шапка .bmp
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
    //RGBTRIPLE rgb;
    uint8_t padding;
    /*bfh.bfType = 19778;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = 0;
    bih.biXPelsPerMeter = 2835;
    bih.biYPelsPerMeter = 2835;
    bih.biClrUsed = 0;
    bih.biClrImportant = 0;*/
    int WOst = initialFiile.WIDTH % Add;
    bih.biWidth = initialFiile.WIDTH / Add;
    if (WOst) ++bih.biWidth;

    int HOst = initialFiile.HIGHT % Add;
    bih.biHeight = initialFiile.HIGHT / Add;
    if (HOst) ++bih.biHeight;

    bfh.bfType = 19778;
    bfh.bfSize = sizeof(bfh) + sizeof(bih) + 3 * bih.biWidth * bih.biWidth;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfOffBits = sizeof(bfh) + sizeof(bih);

    bih.biSize = sizeof(bih);
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = 0;
    bih.biSizeImage = bih.biWidth * bih.biWidth * 3;
    bih.biXPelsPerMeter = 2835;
    bih.biYPelsPerMeter = 2835;
    bih.biClrUsed = 0;
    bih.biClrImportant = 0;

    padding = (4 - (bih.biWidth * 3) % 4) % 4;

    fwrite(&bfh, sizeof(bfh), 1, f2.getF());
    fwrite(&bih, sizeof(bih), 1, f2.getF());//файл не поддерживается(
    
    cout << "было: " << initialFiile.HIGHT << "*" << initialFiile.WIDTH << endl;
    cout << "стало: " << bih.biHeight << "*" << bih.biWidth << endl;
    cout << "выравнивание: " << padding << endl;

    vector<WORD> line1(initialFiile.WIDTH * 3 * Add);
    //vector<WORD> line1(initialFiile.WIDTH * 3 * Add*2);
    vector<BYTE> line2(bih.biWidth * 3 + padding);
    memset(line2.data() + bih.biWidth * 3, 0, padding * sizeof(BYTE));//паддинг line2 черный
    vector <vector<BYTE>> matrBMP(bih.biHeight);

    double AverageB;
    double AverageG;
    double AverageR;

    auto CreatePixel = [&](int allowH, int allowW, int position) {//создаёт каждый отдельный пиксель
        int count = allowH * allowW;
        AverageB = 0;
        AverageG = 0;
        AverageR = 0;
        for (int j = 0; j < allowH; ++j) {
            for (int k = 0; k < allowW; ++k) {
                //суммирование по пикселям
                /*AverageB += line1[j * (initialFiile.WIDTH * 3) + (position * Add + k) * 3] / 256;
                AverageG += line1[j * (initialFiile.WIDTH * 3) + (position * Add + k) * 3 + 1] / 256;
                AverageR += line1[j * (initialFiile.WIDTH * 3) + (position * Add + k) * 3 + 2] / 256;*/

                AverageB += line1[j * (initialFiile.WIDTH * 3) + (position * Add + k) * 3 + 2] / 4;//B
                AverageG += line1[j * (initialFiile.WIDTH * 3) + (position * Add + k) * 3 + 1] / 4;//G
                AverageR += line1[j * (initialFiile.WIDTH * 3) + (position * Add + k) * 3] / 4;//R

                /*AverageB += line1[(j * (initialFiile.WIDTH * 3) + (position * Add + k) * 3)*2+1];
                AverageG += line1[(j * (initialFiile.WIDTH * 3) + (position * Add + k) * 3 + 1)*2+1];
                AverageR += line1[(j * (initialFiile.WIDTH * 3) + (position * Add + k) * 3 + 2)*2+1];*/
            }
        }
        AverageB /= count;
        AverageG /= count;
        AverageR /= count;
        line2.data()[position * 3] = byte(AverageB + 0.5);
        line2.data()[position * 3 + 1] = byte(AverageG + 0.5);
        line2.data()[position * 3 + 2] = byte(AverageR + 0.5);
    };

    auto CreateLine2 = [&](int h1, int w2) {//создаёт всю строку кроме остатка
        for (int i = 0; i < w2; ++i) {
            CreatePixel(h1, Add, i);
        }
    };

    auto CreateAllLine2 = [&](int h1) {
        if (WOst)
        {
            CreateLine2(h1, bih.biWidth - 1);//создаёт всю строку кроме последнего пикселя
            CreatePixel(h1, WOst, bih.biWidth - 1);//последний пиксель в строке
        }
        else
        {
            CreateLine2(h1, bih.biWidth);//создаёт всю строку
        }
    };

    fseek(f1.getF(), initialFiile.start, SEEK_SET);//где хранится ссылка на фактическое начало изображения?
    //fseek(f1.getF(), initialFiile.offsetOfStrips, SEEK_SET);

    for (int i = 0; i < initialFiile.HIGHT / Add; ++i) {
        fread(line1.data(), (initialFiile.WIDTH * 3)* Add*2, 1, f1.getF());
        CreateAllLine2(Add);
        //fwrite(line2.data(), bih.biWidth * 3 + padding, 1, f2.getF());
        matrBMP[i] = line2;
    }

    if (HOst)
    {
        CreateAllLine2(HOst);
        //fwrite(line2.data(), bih.biWidth * 3 + padding, 1, f2.getF());
        matrBMP[bih.biHeight-1] = line2;
    }

    for (int i = bih.biHeight - 1; i >= 0; --i) {
        fwrite(matrBMP[i].data(), bih.biWidth * 3 + padding, 1, f2.getF());
    }
    return 0;
}