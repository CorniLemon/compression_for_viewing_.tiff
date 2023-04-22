#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include<vector>
using namespace std;
#include <stdint.h>
#include "TIFF.h"

typedef unsigned __int16 WORD;//2 �����
//typedef unsigned int DWORD;//4 �����
typedef long LONG;//4 �����
typedef unsigned char BYTE;//1 ����

class FileWithDes {
    FILE* f = NULL;
public:
    FileWithDes(const char A[], const char B[]) {
        f = fopen(A, B);
        if (!f) {
            cout << "���� " << A << " �� ���������� ��� �� ������� �������\n";
            throw 1;
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

int main() {
    setlocale(LC_ALL, "Russian");

    FileWithDes f1("0041_0102_01567_1_01497_03_S_fr.tiff", "rb");//�������� ������ ��� ������ tiff
    //FileWithDes f1("0041_0102_01567_1_01497_03_S.tiff", "rb");//�������� ������ ��� ������ tiff
    TiffFile initialFiile(f1.getF());//������ � tiff

    int Add = 1;
    cout << "����������, ������� ���������� ��������, ������� ����� ���������� � 1 (������� ���������� �������)\n";
    cin >> Add;

    if ((Add >= initialFiile.WIDTH) || (Add >= initialFiile.HIGHT))//�������� add
    {
        cout << "���������� ������������ �������� ������� ������� (����������� �������� � �������� 1*1)";
        return 0;
    }

    FileWithDes f2("final.bmp", "wb");
    //����� .bmp
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
    uint8_t padding;

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
    fwrite(&bih, sizeof(bih), 1, f2.getF());
    
    cout << "����: " << initialFiile.HIGHT << "*" << initialFiile.WIDTH << endl;
    cout << "�����: " << bih.biHeight << "*" << bih.biWidth << endl;
    cout << "������������: " << int(padding) << endl;

    vector<WORD> line1(initialFiile.WIDTH * 3 * Add);
    vector<BYTE> line2(bih.biWidth * 3 + padding);
    memset(line2.data() + bih.biWidth * 3, 0, padding * sizeof(BYTE));//������� line2 ������
    vector <vector<BYTE>> matrBMP(bih.biHeight);

    double AverageB;
    double AverageG;
    double AverageR;

    auto CreatePixel = [&](int allowH, int allowW, int position) {//������ ������ ��������� �������
        int count = allowH * allowW;
        AverageB = 0;
        AverageG = 0;
        AverageR = 0;
        for (int j = 0; j < allowH; ++j) {
            for (int k = 0; k < allowW; ++k) {
                //������������ �� ��������
                AverageB += line1[j * (initialFiile.WIDTH * 3) + (position * Add + k) * 3 + 2] / 4;//B
                AverageG += line1[j * (initialFiile.WIDTH * 3) + (position * Add + k) * 3 + 1] / 4;//G
                AverageR += line1[j * (initialFiile.WIDTH * 3) + (position * Add + k) * 3] / 4;//R
            }
        }
        AverageB /= count;
        AverageG /= count;
        AverageR /= count;
        line2.data()[position * 3] = byte(AverageB + 0.5);
        line2.data()[position * 3 + 1] = byte(AverageG + 0.5);
        line2.data()[position * 3 + 2] = byte(AverageR + 0.5);
    };

    auto CreateLine2 = [&](int h1, int w2) {//������ ��� ������ ����� �������
        for (int i = 0; i < w2; ++i) {
            CreatePixel(h1, Add, i);
        }
    };

    auto CreateAllLine2 = [&](int h1) {
        if (WOst)
        {
            CreateLine2(h1, bih.biWidth - 1);//������ ��� ������ ����� ���������� �������
            CreatePixel(h1, WOst, bih.biWidth - 1);//��������� ������� � ������
        }
        else
        {
            CreateLine2(h1, bih.biWidth);//������ ��� ������
        }
    };

    fseek(f1.getF(), initialFiile.start, SEEK_SET);//��� �������� ������ �� ����������� ������ �����������?

    for (int i = 0; i < initialFiile.HIGHT / Add; ++i) {
        fread(line1.data(), (initialFiile.WIDTH * 3 * 2)* Add, 1, f1.getF());
        CreateAllLine2(Add);
        matrBMP[i] = line2;
    }

    if (HOst)
    {
        CreateAllLine2(HOst);
        matrBMP[bih.biHeight-1] = line2;
    }

    for (int i = bih.biHeight - 1; i >= 0; --i) {
        fwrite(matrBMP[i].data(), bih.biWidth * 3 + padding, 1, f2.getF());
    }
    return 0;
}