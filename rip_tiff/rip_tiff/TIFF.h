#pragma once
#include <iostream>
#include <Windows.h>
#include<vector>
using namespace std;

extern uint64_t histogramR[65536];
extern uint64_t histogramG[65536];
extern uint64_t histogramB[65536];

class TiffFile {
private:
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

    TiffFile(FILE* f);

    void getBrightness(FILE* f);

    void getLine(FILE* f, size_t i, WORD* line1) {//получает номер строки и читает её. fread вызывается
        /*функция работает только в том случае, если getLine вызывается последовательно, для каждой строки файла*/
        int num = i / stringsInStripe;
        if (!(i % stringsInStripe)) {
            fseek(f, startsStrips[num], SEEK_SET);
        }
        fread(line1, (width * 3 * 2), 1, f);
    }  
};