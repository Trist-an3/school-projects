/*Sorting algorithm for the
 * quick sort method
 */

#include "quick.h"

#include <stdio.h>

int partition(Stats *stats, int *A, int low, int high) {
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (cmp(stats, A[j], A[high]) < 0) {
            i += 1;

            swap(stats, &A[j], &A[i]);
        }
    }

    i += 1;

    swap(stats, &A[i], &A[high]);

    return i;
}

void quick_sorter(Stats *stats, int *A, int low, int high) {
    if (low < high) {
        int p = partition(stats, A, low, high);
        quick_sorter(stats, A, low, p - 1);
        quick_sorter(stats, A, p + 1, high);
    }
}

void quick_sort(Stats *stats, int *A, int n) {

    quick_sorter(stats, A, 0, n - 1);
}
