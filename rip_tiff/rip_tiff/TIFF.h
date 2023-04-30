#pragma once
#include <iostream>
#include <Windows.h>
#include<vector>
using namespace std;

class TiffFile {
public:
    size_t HIGHT = 0;//������
    size_t WIDTH = 0;//������
    
    vector<uint32_t> startsStrips;//������ ��������� �� ������ �����
    int countOfStripes = 0;
    int stringsInStripe = 0;
    int countOfChanals = 0;

    TiffFile(FILE* f) {//������ header
        #pragma pack(push, 1)
        struct HeadOfTiff {
            uint16_t IIorMM;
            uint16_t isItTiff;
            uint32_t offset;
        } head;
        #pragma pack(pop)

        uint16_t count;//���������� �����

        #pragma pack(push, 1)
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
        };
        #pragma pack(pop)

        int bitOnPix = 0;//���������� ��� �� �����
        uint32_t startOfPtrToStripes = 0;

        fread(&head, sizeof(head), 1, f);//������ ��������� �����

        fseek(f, head.offset, SEEK_SET);//������� � ������ ������� �����
        fread(&count, 2, 1, f);

        vector<IFD> tags(count);//������ �����

        fread(tags.data(), sizeof(IFD), count, f);//������ �����

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
                if (bitOnPix != 16)
                    throw exception("����� ���������� �� 16 ������!");
                break;
            case 259:
                if (tags[i].valueOffset != 1)
                    throw exception("���� ����!");
                break;
            case 262:
                if (tags[i].valueOffset != 1)
                    throw exception("������ ���������� 255, �.�. ��� ����� �������������");
                break;
            case 273:
                countOfStripes = tags[i].countOfType;
                startOfPtrToStripes = tags[i].valueOffset;
                startsStrips.resize(countOfStripes);
                fseek(f, startOfPtrToStripes, SEEK_SET);
                fread(startsStrips.data(), 4, countOfStripes, f);//4 ������������� ����
                break;
            case 277:
                if (tags[i].valueOffset != 3)
                    throw exception("� ����������� �� 3 ������ � ��� �� ������ ��������� ������������� � ���!");
                break;
            case 278:
                stringsInStripe = tags[i].valueOffset;
                cout << "����� � ������: " << stringsInStripe << endl;
                break;
            default:
                break;
            }
        }

        cout << "���������� �����: " << countOfStripes << endl;

        if (!HIGHT || !WIDTH || !bitOnPix||!startOfPtrToStripes||!sizeof(startsStrips)||!stringsInStripe) {//���� ��� ������-�� ���� 
            throw exception("��� ���� ����������� �����");
        }
        
        cout << "������ = " << HIGHT << endl;
        cout << "������ = " << WIDTH << endl;
        //cout << "�������� � ������ �������� = " << start << endl;
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
    };

    vector<WORD> getLine(FILE* f, size_t i) {
        vector<WORD>  line1(WIDTH * 3);
        int num = i / stringsInStripe;
        if (i % stringsInStripe) {
            ++num;
        }
        else {
            fseek(f, startsStrips[num], SEEK_SET);
        }
        fread(line1.data(), (WIDTH * 3 * 2), 1, f);
        return line1;
    }
        
};