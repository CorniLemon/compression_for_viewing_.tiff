#pragma once
#include <iostream>
#include <Windows.h>
#include<vector>
using namespace std;

inline void bubbleSort(WORD* line, int n) {//слишком долго//inline не очень хорошо, но пусть пока так
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (line[j] > line[j + 1]) {
                WORD temp = line[j];
                line[j] = line[j + 1];
                line[j + 1] = temp;
            }
        }
        //cout << ".";
    }
}

inline void insertionSort(WORD* line, int n) {//слишком долго
    for (int i = 1; i < n; i++)
        for (int j = i; j > 0 && line[j - 1] > line[j]; j--) // пока j>0 и элемент j-1 > j, x-массив int
            swap(line[j - 1], line[j]);
}

inline void radixSortAlgo(WORD* line, int size_of_a) {

    // In the first step (step 1), we fina the maximum value in the array.
    WORD maximumNumber = line[0];

    for (int i = 1; i < size_of_a; i++) {
        maximumNumber = max(maximumNumber, line[i]);
    }

    // In the second step (step 2), we are calculating the number of digits of
    // the maximum element of the array
    int digitsCount = 0;

    while (maximumNumber > 0) {
        digitsCount++;
        maximumNumber /= 10;
    }

    // We are now updating a new array (Steps 3,4 and 5)
    for (int i = 0; i < digitsCount; i++) {
        int pwr = pow(10, i);
        WORD* new_a = new WORD[size_of_a];

        // This is a count_array which is used for the counting array
        // to sort digits 0 to 9.
        int count_array[10];
        memset(count_array, 0, sizeof(count_array));

        // Calculating the frequency of each element of the array
        for (int j = 0; j < size_of_a; j++) {
            int num = (line[j] / pwr) % 10;

            count_array[num]++;
        }

        // This is a comulative frequency
        for (int j = 1; j < 10; j++) {
            count_array[j] += count_array[j - 1];
        }

        // We are mapping the frequency array with each element
        // of the array to find out desired position in the updated array
        for (int j = size_of_a - 1; j >= 0; j--) {
            int num = (line[j] / pwr) % 10;
            new_a[count_array[num] - 1] = line[j];
            count_array[num]--;
        }

        // Now, we are updating the array with the new array
        for (int j = 0; j < size_of_a; j++)
            line[j] = new_a[j];

    }

    // Finally,we print the sorted array result
    for (int j = 0; j < size_of_a; j++)
        cout << line[j] << " ";

    cout << endl;
}