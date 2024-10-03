/*this file is defining all the functions from the 
 * header file operator.h and this is just
 * for all the operatrions and stuff like
 * + - ... sin cos and etc
 */

#include "operators.h"

#include "stack.h"

#include <assert.h>
#include <stdlib.h>

// return false if there are fewer than 2 items on the stack. otherwise, pop rhs, then pop lhs, then
// pass them into op, then push the result. and return true
bool apply_binary_operator(binary_operator_fn op) {
    if (stack_size < 2) {
        return false; // Not enough elements on the stack
    }

    // Pop the first two elements from the stack
    double rhs, lhs;
    if (!stack_pop(&rhs) || !stack_pop(&lhs)) {
        return false; // Failed to pop elements
    }

    // Apply the binary operator function (op) to the operands
    double result = op(lhs, rhs);

    // Push the result back onto the stack
    if (!stack_push(result)) {
        return false; // Failed to push result onto the stack
    }

    return true;
}
// return false if the stack is empty, otherwise pop an item, call op on it, and push the result
bool apply_unary_operator(unary_operator_fn op) {

    // Make sure that the stack is not empty
    if (stack_size < 1) {
        return false;
    }

    double x;
    // It should be impossible for stack_pop to fail here, since we have checked
    // above that the stack is not empty. The assert() function causes your
    // program to exit if the condition passed is false. It is often useful for
    // "sanity checks" like this: if stack_pop fails, either our assumption that
    // it can't fail was false, or the implementation of stack_pop has a bug.
    // Either way, we'd like to know immediately instead of silently ignoring an
    // error.
    assert(stack_pop(&x));
    // Calculate the value we should push to the stack
    double result = op(x);
    // Similar to above: stack_push should never fail here, because we just
    // popped an element, so we are not increasing the stack size past the size
    // it had originally.
    assert(stack_push(result));
    return true;
}

// this function should be given to students
bool parse_double(const char *s, double *d) {

    char *endptr;
    double result = strtod(s, &endptr);

    if (endptr != s) {
        *d = result;
        return true;
    } else {
        return false;
    }
}

double operator_add(double lhs, double rhs) {

    double result = lhs + rhs;
    return result;
}

double operator_sub(double lhs, double rhs) {
    double result = lhs - rhs;
    return result;
}

double operator_mul(double lhs, double rhs) {
    double result = lhs * rhs;
    return result;
}

double operator_div(double lhs, double rhs) {
    double result = lhs / rhs;
    return result;
}
