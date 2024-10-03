/*Sorting algorithm for the 
 * insertion sort function
 */

#include "insert.h"

#include <stdio.h>

void insertion_sort(Stats *stats, int *arr, int length) {

    for (int i = 1; i < length; i++) {
        int j = i;
        int temp = move(stats, arr[i]);

        while (j >= 1 && cmp(stats, temp, arr[j - 1]) < 0) {
            arr[j] = move(stats, arr[j - 1]);
            j -= 1;
        }

        arr[j] = move(stats, temp);
    }
}
