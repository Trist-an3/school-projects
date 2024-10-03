#include "stack.h"

#include <assert.h>
#include <stdlib.h>

typedef struct stack {
    uint32_t capacity;
    uint32_t top;
    uint32_t *items;
} Stack;

Stack *stack_create(uint32_t capacity) {

    Stack *s = (Stack *) malloc(sizeof(Stack));
    s->capacity = capacity;
    s->top = 0;
    // We need enough memory for <capacity> numbers
    s->items = calloc(s->capacity, sizeof(uint32_t));
    // We created our stack, return it!
    return s;
}

void stack_free(Stack **sp) {
    // Check if the double pointer and the pointer it points to are not null
    if (sp != NULL && *sp != NULL) {
        // Check if the array of items is allocated
        if ((*sp)->items) {
            // Free the memory for the array of items
            free((*sp)->items);
            // Set the pointer to NULL to avoid accessing freed memory
            (*sp)->items = NULL;
        }
        // Free memory allocated for the stack structure
        free(*sp);
    }

    // Set the pointer to NULL to avoid double-free
    if (sp != NULL) {
        *sp = NULL;
    }
}

bool stack_push(Stack *s, uint32_t val) {
    // If the stack is full, return false;
    if (stack_full(s)) {
        return false;
    }

    // Set val
    s->items[s->top] = val;
    // Move the top of the stack
    s->top++;
    return true;
}

bool stack_pop(Stack *s, uint32_t *val) {
    if (stack_empty(s)) {
        return false;
    }

    //move top of stack to last items index
    s->top--;

    //this is setting the new val
    *val = s->items[s->top];
    //means success
    return true;
}

bool stack_peek(const Stack *s, uint32_t *val) {
    if (stack_empty(s)) {
        return false;
    }

    //checks to see the value of last element in the stack
    *val = s->items[s->top - 1];

    //peek success
    return true;
}

bool stack_empty(const Stack *s) {
    //checks if stack is empty
    if ((s->top) == 0) {
        return true;
    }

    return false;
}

bool stack_full(const Stack *s) {
    //checks if # of elements is equal to capacity
    if ((s->top) == s->capacity) {
        return true;
    }

    //return false if not equal to capcity
    return false;
}

uint32_t stack_size(const Stack *s) {
    return s->top;
}

void stack_copy(Stack *dst, const Stack *src) {
    //checks if dst has enuf space to store items from src
    assert((dst->capacity) >= src->top);

    //copy items from src to dst
    for (uint32_t i = 0; i < src->top; i++) {
        dst->items[i] = src->items[i];
    }

    dst->top = src->top;
}

void stack_print(const Stack *s, FILE *f, char *vals[]) {
    for (uint32_t i = 0; i < s->top; i += 1) {
        fprintf(f, "%s\n", vals[s->items[i]]);
    }
}
