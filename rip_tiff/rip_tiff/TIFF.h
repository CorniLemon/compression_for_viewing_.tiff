#pragma once
#include <iostream>
#include <Windows.h>
#include<vector>
using namespace std;

class TiffFile {
public:
    struct HeadOfTiff {
        uint16_t IIorMM;
        uint16_t isItTiff;
        uint32_t offset;
    } head;
    uint16_t count;
    struct IFD {
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
     }ifd;

    vector<IFD> ifdS;

    size_t HIGHT;
    size_t WIDTH;
    uint32_t offsetOfStrips;
    int zzz;
    size_t stac;
    size_t stac1;
    uint32_t start;

    TiffFile(FILE* f) {//читает header
        fread(&head.IIorMM, 2, 1, f);
        fread(&head.isItTiff, 2, 1, f);
        fread(&head.offset, 4, 1, f);
        //stac1 = SEEK_CUR;
        stac1 = ftell(f);

        fseek(f, head.offset, SEEK_SET);

        fread(&count, 2, 1, f);

        //count = 5;//потом обязательно удалить!!!!!!!!!!!!
        ifdS.reserve(count);//размер вектора

        for (int i = 0; i < count; ++i)
        {
            fread(&ifd.tag, 2, 1, f);
            fread(&ifd.type, 2, 1, f);
            fread(&ifd.countOfType, 4, 1, f);
            fread(&ifd.valueOffset, 4, 1, f);

            ifdS.push_back(ifd);

            switch (ifd.tag)
            {
            case 256:
                WIDTH = ifd.valueOffset;
                break;
            case 257:
                HIGHT = ifd.valueOffset;
                break;
            case 258:
                //stac = SEEK_CUR;//не прокатило
                stac = ifd.valueOffset;
                /*fseek(f, ifd.valueOffset, SEEK_SET);
                fread(&zzz, 4, 1, f);
                fseek(f, stac, SEEK_SET);*/
                break;
            case 273:
                offsetOfStrips = ifd.valueOffset;
                break;
            default:
                break;
            }
        }

        fseek(f, stac, SEEK_SET);
        fread(&zzz, 2, 1, f);

        
        fseek(f, offsetOfStrips, SEEK_SET);
        fread(&start, 4, 1, f);

        cout << "высота = " << HIGHT << endl;
        cout << "ширина = " << WIDTH << endl;
        cout << "смещение к началу пикселей = " << start << endl;
        cout << "бит на канал = " << zzz << endl;

        //print();
    };

    void print() {
        cout << "IIorMM = " << head.IIorMM << endl;
        cout << "isItTiff = " << head.isItTiff << endl;
        cout << "offset = " << head.offset << endl;
        cout << endl;
        cout << "count = " << count << endl;
        cout << endl;
        for (IFD ifdOut : ifdS) {
            ifdOut.printIfd();
            cout << endl;
        }
    };

};