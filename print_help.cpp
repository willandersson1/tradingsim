#include "print_help.h"

void printArr(int n, int A []) {
    for (int i = 0; i < n - 1; i++) {
        std::cout << A[i] << "  ";
    }

    std::cout << A[n - 1] << std::endl;
}

void printMat(int m, int n, int *A) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n - 1; j++) {
            std::cout << A[j + i * n] << "  ";
        }

        std::cout << A[n - 1 + i * n] << std::endl;
    }
}