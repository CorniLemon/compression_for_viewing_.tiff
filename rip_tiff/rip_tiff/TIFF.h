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
public:
    size_t height = 0;//������
    size_t width = 0;//������
    
    vector<uint32_t> startsStrips;//������ ��������� �� ������ �����
    int countOfStripes = 0;
    int stringsInStripe = 0;
    int countOfChanals = 0;

    int maxR = 65535;
    int minR = 0;
    int maxG = 65535;
    int minG = 0;
    int maxB = 65535;
    int minB = 0;

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
        struct IFD {//����
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

        for (int i = 0; i < count; ++i)//����� ����������� �����
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
                startsStrips.resize(countOfStripes);
                if (countOfStripes == 1) {
                    startsStrips[0] = tags[i].valueOffset;
                }
                else {
                    startOfPtrToStripes = tags[i].valueOffset;
                    fseek(f, startOfPtrToStripes, SEEK_SET);
                    fread(startsStrips.data(), 4, countOfStripes, f);//4 ������������� ����
                }
                break;
            case 277:
                countOfChanals = tags[i].valueOffset;
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

        if (!height || !width || !bitOnPix||!startOfPtrToStripes||!sizeof(startsStrips)||!stringsInStripe||!countOfChanals) {//�������� ������� �������� ����� 
            throw exception("��� ���� ����������� �����");
        }

        cout << "���������� �����: " << countOfStripes << endl;
        cout << "������ = " << height << endl;
        cout << "������ = " << width << endl;
        cout << "��� �� ����� = " << bitOnPix << endl;

        R.resize(height* width);
        G.resize(height* width);
        B.resize(height* width);

        auto print = [&]() {//����� � [], �.�. ��� ������ �� ����� ��������� � ������ ����� �������� ������������
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

    void bubbleSort(WORD* line, int n) {
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (line[j] > line[j + 1]) {
                    int temp = line[j];
                    line[j] = line[j + 1];
                    line[j + 1] = temp;
                }
            }
        }
    }

    void getBrightness(FILE* f) {
        vector<WORD> line (width * 3);
        for (int i = 0; i < height; ++i) {//������
            getLine(f, i, line.data());
            for (int j = 0; j < width; ++j) {
                R[i * width + j] = line[j * 3];
                G[i * width + j] = line[j * 3 + 1];
                B[i * width + j] = line[j * 3 + 2];
            }
        }
        /*����� ��������*/
        
        int Q1, Q3;
        Q1 = width / 4;
        Q3 = Q1 * 3;
        //cout << R[0] << " " << R[Q1] << " " << R[Q3] << " " << R[width - 1] << endl;

        bubbleSort(R.data(), width);
        bubbleSort(G.data(), width);
        bubbleSort(B.data(), width);

        /*���������� �������*/
        minR = R[Q1] - ((R[Q3] - R[Q1]) * 3);
        maxR = R[Q3] + ((R[Q3] - R[Q1]) * 3);
        minG = G[Q1] - ((G[Q3] - G[Q1]) * 3);
        maxG = G[Q3] + ((G[Q3] - G[Q1]) * 3);
        minB = B[Q1] - ((B[Q3] - B[Q1]) * 3);
        maxB = B[Q3] + ((B[Q3] - B[Q1]) * 3);
        /*minR = R[Q1] - ((R[Q3] - R[Q1]) * 1.5);
        maxR = R[Q3] + ((R[Q3] - R[Q1]) * 1.5);
        minG = G[Q1] - ((G[Q3] - G[Q1]) * 1.5);
        maxG = G[Q3] + ((G[Q3] - G[Q1]) * 1.5);
        minB = B[Q1] - ((B[Q3] - B[Q1]) * 1.5);
        maxB = B[Q3] + ((B[Q3] - B[Q1]) * 1.5);*/
        cout << "\n�������� R: " << minR << ":" << maxR;
        cout << "\n�������� G: " << minG << ":" << maxG;
        cout << "\n�������� B: " << minB << ":" << maxB<<"\n";

        /*cout << "\n�����������" << 255 / (maxR - minR) << endl;
        cout << "\n�����������" << 255 / (maxG - minG) << endl;
        cout << "\n�����������" << 255 / (maxB - minB) << endl;*/

        cout << R[0] << " " << R[Q1] << " " << R[Q3] << " " << R[width - 1] << endl;
        cout << G[0] << " " << G[Q1] << " " << G[Q3] << " " << G[width - 1] << endl;
        cout << B[0] << " " << B[Q1] << " " << B[Q3] << " " << B[width - 1] << endl;
        /*cout << "\n ����� ��������: " << width;
        cout << "\n Q1: " << Q1;
        cout << "\n Q3: " << Q3;*/
    }

    void getLine(FILE* f, size_t i, WORD* line1) {//�������� ����� ������ � ������ �. fread ����������
        /*������� �������� ������ � ��� ������, ���� getLine ���������� ���������������, ��� ������ ������ �����*/
        int num = i / stringsInStripe;
        if (!(i % stringsInStripe)) {
            fseek(f, startsStrips[num], SEEK_SET);
        }
        fread(line1, (width * 3 * 2), 1, f);
    }  
};