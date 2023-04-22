#pragma once
#include <iostream>
#include <Windows.h>
#include<vector>
using namespace std;

class TiffFile {
public:
    size_t HIGHT;
    size_t WIDTH;
    int bitOnPix;
    uint32_t start;

    TiffFile(FILE* f) {//������ header
        #pragma pac(push, 1);
        struct HeadOfTiff {
            uint16_t IIorMM;
            uint16_t isItTiff;
            uint32_t offset;
        } head;
        #pragma pac(pop);

        uint16_t count;

        #pragma pac(push, 1);
        struct IFD {
            uint16_t tag;
            uint16_t type;
            uint32_t countOfType;
            uint32_t valueOffset;
            void printIfd() {
                cout << "tag = " << tag << endl;
                cout << "type = " << type << endl;//������ 120 ����� �������� 12?
                cout << "countOfType = " << countOfType << endl;
                cout << "valueOffset = " << valueOffset << endl;
            }
        }ifd;
        #pragma pac(pop);

        fread(&head, sizeof(head), 1, f);

        fseek(f, head.offset, SEEK_SET);
        fread(&count, 2, 1, f);

        IFD* tags = new IFD[count];

        fread(tags, sizeof(ifd), count, f);

        for (int i = 0; i < count; ++i)
        {
            switch (tags[i].tag)
            {
            case 256:
                WIDTH = tags[i].valueOffset;
                break;
            case 257:
                HIGHT = tags[i].valueOffset;
                break;
            case 258:
                fseek(f, tags[i].valueOffset, SEEK_SET);
                fread(&bitOnPix, 2, 1, f);//2 ������������� ����
                break;
            case 273:
                fseek(f, tags[i].valueOffset, SEEK_SET);
                fread(&start, 4, 1, f);//4 ������������� ����
                break;
            default:
                break;
            }
        }

        cout << "������ = " << HIGHT << endl;
        cout << "������ = " << WIDTH << endl;
        cout << "�������� � ������ �������� = " << start << endl;
        cout << "��� �� ����� = " << bitOnPix << endl;

        auto print = [&]() {
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

        delete[] tags;
    };
};