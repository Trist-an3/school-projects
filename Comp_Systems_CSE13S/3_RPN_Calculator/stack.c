/*C file for the stack.h header file
 * pretty much defining all the functions from there there
 * this takes care of the stack and positioning stuff of the 
 * calculator
 */

#include "stack.h"

#include <stdio.h>

double stack[STACK_CAPACITY] = { 0 };
int stack_size = 0;

bool stack_push(double item) {
    // Implement the logic to push an item onto the stack
    // Check for stack overflow

    if (stack_size == STACK_CAPACITY) {
        return false;
    } else {
        stack[stack_size] = item;
        stack_size++;
        return true;
    }
}

bool stack_peek(double *item) {
    // Implement the logic to peek at the top item on the stack
    // Check for an empty stack

    if (stack_size == 0) {
        return false; // stack empty cant peak
    } else {
        *item = stack[stack_size - 1]; //accesses latest addition to stack
        return true;
    }
}

bool stack_pop(double *item) {
    // Implement the logic to pop an item from the stack
    // Check for an empty stack
    if (stack_size == 0) {
        return false;
    } else {
        *item = stack[stack_size - 1];
        stack_size--;
        return true;
    }
}

void stack_clear(void) {
    // Implement the logic to clear the stack
    stack_size = 0;
}

void stack_print(void) {

    // make sure we don't print stack[0] when it is empty
    if (stack_size == 0) {
        return;
    }

    // print the first element with 10 decimal places
    printf("%.10f", stack[0]);

    // print the remaining elements (if any), with a space before each one
    for (int i = 1; i < stack_size; i++) {
        printf(" %.10f", stack[i]);
    }
}
