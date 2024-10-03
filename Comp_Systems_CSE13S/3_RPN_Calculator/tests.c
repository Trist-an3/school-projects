#include "mathlib.h"
#include "messages.h"
#include "operators.h"
#include "stack.h"

#include <assert.h>
#include <stdio.h>

bool TestMathlibFunctions(void) {
    bool success = true;
    double result;
    double expected;

    result = Abs(-5.0);
    assert(result == 5.0);

    result = Sqrt(49.0);
    assert(result == 7.0);

    result = Sin(10);
    expected = -0.54402111088;
    assert(fabs(result - expected) < 0.00000000001);

    result = Cos(10);
    expected = -0.83907152907;
    assert(fabs(result - expected) < 0.00000000001);

    result = Tan(10);
    expected = 0.64836082745;
    assert(fabs(result - expected) < 0.00000000001);

    return success;
}

bool TestStackFunctions(void) {
    bool success = true;
    double result;

    //testing push and peek
    stack_push(5.0);
    stack_push(10.0);
    assert(stack_peek(&result) && result == 10.0);

    stack_pop(&result);
    assert(stack_size == 1);

    stack_clear();
    assert(stack_size == 0);

    return success;
}
bool TestOperatorFunctions(void) {
    bool success = true;
    double result;
    double input;

    stack_push(5.0);
    stack_push(3.0);
    apply_binary_operator(operator_add);
    stack_peek(&result);
    assert(result == 8.0);

    stack_clear();

    stack_push(90.0);
    result = sin(90.0);
    input = Sin(90.0);
    apply_unary_operator(Sin);
    assert(fabs(result - input) < 0.00000000001);

    // Test parsing a valid double
    assert(parse_double("3.14", &result) && result == 3.14);

    // Test parsing a negative double
    assert(parse_double("-10.5", &result) && result == -10.5);

    // Test parsing an integer
    assert(parse_double("42", &result) && result == 42.0);

    // Test addition function
    assert(operator_add(2.0, 3.0) == 5.0);
    assert(operator_add(4.0, -2.0) == 2.0);

    // Test subtraction function
    assert(operator_sub(5.0, 2.0) == 3.0);
    assert(operator_sub(-2.0, -5.0) == 3.0);

    // Test multiplication function
    assert(operator_mul(2.0, 3.0) == 6.0);
    assert(operator_mul(4.0, -2.0) == -8.0);

    // Test division function
    assert(operator_div(10.0, 2.0) == 5.0);
    assert(operator_div(5.0, 0.0));
    assert(operator_div(-6.0, 2.0) == -3.0);

    return success;
}

int main(void) {

    if (TestMathlibFunctions()) {
        printf("Mathlib's functions Passes!\n");
    }

    if (TestStackFunctions()) {
        printf("Stack's functions passed!\n");
    }

    if (TestOperatorFunctions()) {
        printf("Operator's functions passed!\n");
    }

    return 0;
}
