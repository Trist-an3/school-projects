/*Tristan Garcia
 *CSE13s 
 *This is the main file for the 
 *sorting and sets assignment
 *User types in what algorithm they want to use
 *and set specifications
 */

#include "batcher.h"
#include "gaps.h"
#include "heap.h"
#include "insert.h"
#include "quick.h"
#include "set.h"
#include "shell.h"
#include "stats.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define OPTIONS                "abhin:p:qr:sH"
#define DEFAULT_LENGTH         100
#define DEFAULT_PRINT_ELEMENTS 100
#define DEFAULT_SEED           13371453

int main(int argc, char **argv) {
    int opt = 0;

    int length = DEFAULT_LENGTH;
    int print_elements = DEFAULT_PRINT_ELEMENTS;
    int seed = DEFAULT_SEED;
    Set set_count = set_empty();

    Stats stats;
    reset(&stats);

    bool option_a = false;
    bool option_b = false;
    bool option_h = false;
    bool option_i = false;
    bool option_q = false;
    bool option_s = false;
    bool option_H = false;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'a':
            option_a = true;
            set_insert(set_count, 0); // insert
            set_insert(set_count, 1); // heap
            set_insert(set_count, 2); // shell
            set_insert(set_count, 3); // quick
            set_insert(set_count, 4); // batcher
            break;

        case 'b':
            option_b = true;
            set_insert(set_count, 0);
            break;

        case 'h':
            option_h = true;
            set_insert(set_count, 1);
            break;

        case 'i':
            option_i = true;
            set_insert(set_count, 2);
            break;

        case 'n': length = atoi(optarg); break;

        case 'p': print_elements = atoi(optarg); break;

        case 'q':
            option_q = true;
            set_insert(set_count, 3);
            break;

        case 'r':
            seed = atoi(optarg);
            srand(seed);
            break;

        case 's':
            option_s = true;
            set_insert(set_count, 4);
            break;

        case 'H': option_H = true; break;

        default: option_H = true; break;
        }
    }

    if (!option_a && !option_b && !option_h && !option_i && !option_q && !option_s) {

        printf("Select at least one sort to perform.\n");
        option_H = true;
    }

    if (option_H) {
        printf("SYNOPIS\n");
        printf("   A collection of comparison-based sorting algorithms.\n\n");
        printf("USAGE\n   ./sorting [-Hahbsqi] [-n length] [-p elements] [-r seed]\n\n");
        printf("OPTIONS\n");
        printf("   -H              Display program help and usage.\n");
        printf("   -a              Enable all sorts.\n");
        printf("   -h		   Enable heap Sort.\n");
        printf("   -b		   Enable Batcher Sort.\n");
        printf("   -s		   Enable Shell Sort.\n");
        printf("   -q		   Enable Quick Sort.\n");
        printf("   -i		   Enable Insertion Sort.\n");
        printf("   -n length       Specify number of array elements (default: 100).\n");
        printf("   -p elements     Specify number of elements to print (default: 100).\n");
        printf("   -r seed         Specify random seed (default: 13371453).\n");
    }

    srand(seed);

    //memory allocation for arrays
    int *random_array = calloc(length, sizeof(int));
    int *batcher_array = calloc(length, sizeof(int));
    int *heap_array = calloc(length, sizeof(int));
    int *insert_array = calloc(length, sizeof(int));
    int *quick_array = calloc(length, sizeof(int));
    int *shell_array = calloc(length, sizeof(int));

    //fills the random array with random integer values
    for (int i = 0; i < length; i++) {
        int randomizer = random() & 0x3FFFFFFF;
        random_array[i] = randomizer;
    }

    //copy elements from random_array into other strings
    for (int i = 0; i < length; i++) {
        batcher_array[i] = random_array[i];
        heap_array[i] = random_array[i];
        insert_array[i] = random_array[i];
        quick_array[i] = random_array[i];
        shell_array[i] = random_array[i];
    }

    if (option_a || option_i) {

        //set member 0 in set count variable to enable sort algo
        set_member(set_count, 2);

        //if # of elements to print is greater than length then
        //have print_element = to length
        if (print_elements > length) {
            print_elements = length;
        }

        //does the sorting magic and prints stats
        insertion_sort(&stats, insert_array, length);
        print_stats(&stats, "Insertion Sort", length);

        //prints the number of elemnts in the print_elements, and then formats them in coloumn
        for (int i = 0; i < print_elements; i++) {
            if ((i != 0) && (i % 5) == 0) {
                printf("\n");
            }
            printf("%13" PRIu32, insert_array[i]);
        }

        //prints a newline if there are elements to display
        if (print_elements != 0) {
            printf("\n");
        }

        reset(&stats);
        //free(insert_array);
    }

    if (option_a || option_h) {
        set_member(set_count, 1);

        if (print_elements > length) {
            print_elements = length;
        }

        heap_sort(&stats, heap_array, length);
        print_stats(&stats, "Heap Sort", length);

        for (int i = 0; i < print_elements; i++) {
            if ((i != 0) && (i % 5) == 0) {
                printf("\n");
            }
            printf("%13" PRIu32, heap_array[i]);
        }

        if (print_elements != 0) {
            printf("\n");
        }

        reset(&stats);
    }

    if (option_a || option_s) {
        set_member(set_count, 4);

        if (print_elements > length) {
            print_elements = length;
        }

        shell_sort(&stats, shell_array, length);
        print_stats(&stats, "Shell Sort", length);

        for (int i = 0; i < print_elements; i++) {
            if (i != 0 && (i % 5) == 0) {
                printf("\n");
            }

            printf("%13" PRIu32, shell_array[i]);
        }

        if (print_elements != 0) {
            printf("\n");
        }

        reset(&stats);
    }

    if (option_a || option_q) {

        set_member(set_count, 3);

        if (print_elements > length) {
            print_elements = length;
        }

        quick_sort(&stats, quick_array, length);
        print_stats(&stats, "Quick Sort", length);

        for (int i = 0; i < print_elements; i++) {
            if (i != 0 && (i % 5) == 0) {
                printf("\n");
            }
            printf("%13" PRIu32, quick_array[i]);
        }

        if (print_elements != 0) {
            printf("\n");
        }

        reset(&stats);
    }

    if (option_a || option_b) {

        set_member(set_count, 0);

        if (print_elements > length) {
            print_elements = length;
        }

        batcher_sort(&stats, batcher_array, length);
        print_stats(&stats, "Batcher Sort", length);

        for (int i = 0; i < print_elements; i++) {
            if ((i != 0) && (i % 5 == 0)) {
                printf("\n");
            }
            printf("%13" PRIu32, batcher_array[i]);
        }

        if (print_elements != 0) {
            printf("\n");
        }

        reset(&stats);
    }

    free(insert_array);
    free(heap_array);
    free(shell_array);
    free(quick_array);
    free(batcher_array);
    free(random_array);
    return 0;
}
