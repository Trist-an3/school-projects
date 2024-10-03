/*Sorting algorithm for the
 * shell sort function
 */

#include "shell.h"

#include "gaps.h"

#include <stdio.h>

void shell_sort(Stats *stats, int *A, int n) {

    for (int gapIndex = 0; gapIndex < GAPS; gapIndex++) {

        int gap = gaps[gapIndex];

        for (int k = gap; k < n; k++) {

            int temp = move(stats, A[k]);
            int j = k;

            while (j >= gap && cmp(stats, temp, A[j - gap]) < 0) {
                A[j] = move(stats, A[j - gap]);
                j -= gap;
            }

            A[j] = move(stats, temp);
        }
    }
}
