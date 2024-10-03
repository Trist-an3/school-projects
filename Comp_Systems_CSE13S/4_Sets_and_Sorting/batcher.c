/*Sorting algorithm for the 
 * batcher functoin
 */

#include "batcher.h"

#include <stdio.h>

void comparator(Stats *stats, int *A, int x, int y) {

    if (cmp(stats, A[x], A[y]) > 0) {
        swap(stats, &A[x], &A[y]);
    }
}

void batcher_sort(Stats *stats, int *A, int n) {

    if (n == 0) {
        return;
    }

    int bit_length = n;
    int t = 0;

    while (bit_length > 0) {
        t++;
        bit_length >>= 1;
    }

    int p = 1 << (t - 1);

    while (p > 0) {
        int q = 1 << (t - 1);
        int r = 0;
        int d = p;

        while (d > 0) {
            for (int i = 0; i < n - d; i++) {
                if ((i & p) == r) {
                    comparator(stats, A, i, i + d);
                }
            }

            d = q - p;
            q >>= 1;
            r = p;
        }

        p >>= 1;
    }
}
