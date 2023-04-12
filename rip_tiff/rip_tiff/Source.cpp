#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
using namespace std;

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
            throw;
        }
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

    //шапка .bmp
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
    RGBTRIPLE rgb;
    size_t padding;
    bfh.bfType = 19778;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = 0;
    bih.biXPelsPerMeter = 2835;
    bih.biYPelsPerMeter = 2835;
    bih.biClrUsed = 0;
    bih.biClrImportant = 0;
    FileWithDes f2("final.bmp", "wb");

    int Add = 1;
    cout << "пожалуйста, введите количество пикселей, которые будут объединены в 1 (сторона квадратной области)\n";
    cin >> Add;

    if ((Add + 1 > bih.biWidth) || (Add + 1 > bih.biHeight))
    {
        cout << "количество объединяемых пикселей слишком большое (изображение сожмется в картинку 1*1)";
        return 0;
    }

    fwrite(&bfh, sizeof(bfh), 1, f2.getF());
    fwrite(&bih, sizeof(bih), 1, f2.getF());

    /*LONG WIGTH = bih.biWidth;//заполнение шапки .bmp
    int WOst = bih.biWidth % Add;
    bih.biWidth = bih.biWidth / Add;
    if (WOst) ++bih.biWidth;

    LONG HEIGHT = bih.biHeight;
    int HOst = bih.biHeight % Add;
    bih.biHeight = bih.biHeight / Add;
    if (HOst) ++bih.biHeight;

    DWORD SIZE = bih.biSizeImage;
    bih.biSizeImage = bih.biHeight * bih.biWidth;*/

    cout << "стало: " << bih.biHeight << "*" << bih.biWidth << endl;

    padding = (4 - (bih.biWidth * 3) % 4) % 4;
    bfh.bfSize = sizeof(bfh) + sizeof(bih) + bih.biSizeImage * 3 + padding * bih.biHeight * 3;

    Line line2(bih.biWidth * 3 + padding);

    memset(line2.data() + bih.biWidth * 3, 0, padding * sizeof(BYTE));//паддинг line2 черный

    cout << "padding.after: " << padding << endl;

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
        //if (WOst)
        //{
            CreateLine2(h1, bih.biWidth - 1);//создаёт всю строку кроме последнего пикселя
            //CreatePixel(h1, WOst, bih.biWidth - 1);//последний пиксель в строке
        //}
        //else
        //{
        //    CreateLine2(h1, bih.biWidth);//создаёт всю строку
        //}
    };

    for (int i = 0; i < HEIGHT / Add; ++i) {
        
        CreateAllLine2(Add);
        fwrite(line2.data(), bih.biWidth * 3 + padding, 1, f2.getF());
    }
    /*if (HOst)
    {
        CreateAllLine2(HOst);
        fwrite(line2.data(), bih.biWidth * 3 + padding.after, 1, f2.getF());
    }*/
    return 0;
}