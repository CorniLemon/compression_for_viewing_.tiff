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
            throw exception("���� � ����� ������ �� ���������� ��� �� ������� �������\n");
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

    try {
        FileWithDes f1("0041_0102_01567_1_01497_03_S_fr.tiff", "rb");//�������� ������ ��� ������ ���������� tiff
        //FileWithDes f1("0041_0102_01567_1_01497_03_S.tiff", "rb");//�������� ������ ��� ������ �������� tiff
        //FileWithDes f1("ket.tiff", "rb");//�������� ������ ��� ������ tiff � �������� (�� ��������)
        TiffFile initialFile(f1.getF());//������ � tiff
        

        int Add = 1;
        cout << "����������, ������� ���������� ��������, ������� ����� ���������� � 1 (������� ���������� �������)\n";
        cin >> Add;

        if ((Add >= initialFile.width) || (Add >= initialFile.height))//�������� add
        {
            throw exception("���������� ������������ �������� ������� ������� (����������� �������� � �������� 1*1)");
        }

        FileWithDes f2("final.bmp", "wb");
        //����� .bmp
        BITMAPFILEHEADER bfh;
        BITMAPINFOHEADER bih;
        uint8_t padding;

        int WOst = initialFile.width % Add;
        bih.biWidth = initialFile.width / Add;
        if (WOst) ++bih.biWidth;

        int HOst = initialFile.height % Add;
        bih.biHeight = initialFile.height / Add;
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

        cout << "����: " << initialFile.height << "*" << initialFile.width << endl;
        cout << "�����: " << bih.biHeight << "*" << bih.biWidth << endl;
        cout << "������������: " << int(padding) << endl;

        vector<WORD> line1(initialFile.width * 3 * Add);

        vector<BYTE> line2(bih.biWidth * 3 + padding);
        memset(line2.data() + bih.biWidth * 3, 0, padding * sizeof(BYTE));//������� line2 ������

        vector <vector<BYTE>> matrBMP(bih.biHeight);//���� bmp

        //cout << "�������� ����� 1\n";
              
        auto CreatePixel = [&](int allowH, int allowW, int position) {//������ ������ ��������� �������
            int count = allowH * allowW;
            double AverageB;
            double AverageG;
            double AverageR;
            AverageB = 0;
            AverageG = 0;
            AverageR = 0;
            for (int j = 0; j < allowH; ++j) {
                for (int k = 0; k < allowW; ++k) {
                    //������������ �� ��������
                    if ((line1[j * (initialFile.width * 3) + (position * Add + k) * 3]) < initialFile.minR)
                        AverageR += 0;
                    else if ((line1[j * (initialFile.width * 3) + (position * Add + k) * 3]) > initialFile.maxR)
                        AverageR += 255;
                    else
                        AverageR += ((line1[j * (initialFile.width * 3) + (position * Add + k) * 3]) - initialFile.minR) * 255ll / (initialFile.maxR - initialFile.minR);//R

                    if ((line1[j * (initialFile.width * 3) + (position * Add + k) * 3 + 1]) < initialFile.minG)
                        AverageG += 0;
                    else if ((line1[j * (initialFile.width * 3) + (position * Add + k) * 3 + 1]) > initialFile.maxG)
                        AverageG += 255;
                    else
                        AverageG += ((line1[j * (initialFile.width * 3) + (position * Add + k) * 3 + 1]) - initialFile.minG) * 255ll / (initialFile.maxG - initialFile.minG);//G

                    if ((line1[j * (initialFile.width * 3) + (position * Add + k) * 3 + 2]) < initialFile.minB)
                        AverageB += 0;
                    else if ((line1[j * (initialFile.width * 3) + (position * Add + k) * 3 + 2]) > initialFile.maxB)
                        AverageB += 255;
                    else
                        AverageB += ((line1[j * (initialFile.width * 3) + (position * Add + k) * 3 + 2]) - initialFile.minB) * 255ll / (initialFile.maxB - initialFile.minB);//B 
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

        initialFile.getBrightness(f1.getF());

        //cout << "�������� ����� 2\n";
        
        for (int i = 0; i < initialFile.height / Add; ++i) {
            for (int j = 0; j < Add; ++j) {
                initialFile.getLine(f1.getF(), i * Add + j, line1.data()+ j * (initialFile.width * 3));
            }
            CreateAllLine2(Add);
            matrBMP[i] = line2;
        }

        //cout << "�������� ����� 3\n";

        if (HOst)
        {
            for (int j = 0; j < HOst; ++j) {
                initialFile.getLine(f1.getF(), (initialFile.height / Add) * Add + j, line1.data() + j * (initialFile.width * 3));
            }
            CreateAllLine2(HOst);
            matrBMP[bih.biHeight - 1] = line2;
        }

        //cout << "�������� ����� 4\n";

        for (int i = bih.biHeight - 1; i >= 0; --i) {
            fwrite(matrBMP[i].data(), bih.biWidth * 3 + padding, 1, f2.getF());
        }

        //cout << "�������� ����� 5\n";
    }
    catch (exception& ex) {
        cout << "���-�� ����� �� ���...\n\n";
        cout<<ex.what()<<endl;
    };
    return 0;
}