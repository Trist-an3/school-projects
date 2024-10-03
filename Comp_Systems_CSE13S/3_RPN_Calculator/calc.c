/*Tristan Garcia
 * CSE13s
 * This program is an RPN calculator
 * it uses various header files like messages.h stack.h
 * mathlib.h and stack.h.
 * And we make different c files to delcare all the functions 
 * Test will also be made to check if all functions pass the test
 */

#include "mathlib.h"
#include "messages.h"
#include "operators.h"
#include "stack.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int opt;

    bool option_m = false;
    bool option_h = false;

    unary_operator_fn sin_function;
    unary_operator_fn cos_function;
    unary_operator_fn tan_function;
    unary_operator_fn abs_function;
    unary_operator_fn sqrt_function;

    while ((opt = getopt(argc, argv, "mh")) != -1) {
        switch (opt) {
        case 'm': option_m = true; break;

        case 'h': option_h = true; break;

        default: printf("%s", USAGE); exit(1);
        }
    }

    if (option_h) {
        printf("%s\n", USAGE);
    } else {
        if (option_m) {
            // Use standard math library functions
            // sin_function = sin;
            // cos_function = cos;
            // tan_function = tan;
            // abs_function = fabs;
            // sqrt_function = sqrt;
        } else {
            // Use your custom functions
            sin_function = Sin;
            cos_function = Cos;
            tan_function = Tan;
            abs_function = Abs;
            sqrt_function = Sqrt;
        }
    }

    while (1) {

        fprintf(stderr, "> ");

        char input[256];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // Handle input error (e.g., EOF)
            break;
        }
        char *saveptr;
        char *token = strtok_r(input, " ", &saveptr);
        double value;

        while (token != NULL) {
            if (parse_double(token, &value)) {

                // Token is a valid number, push it onto the stack
                stack_push(value);

                //check token if valid
            } else if (strlen(token) == 1
                       && (token[0] == '+' || token[0] == '-' || token[0] == '*' || token[0] == '/'
                           || token[0] == '%' || token[0] == 's' || token[0] == 'c'
                           || token[0] == 't' || token[0] == 'a' || token[0] == 'r')) {

                char op = token[0];

                //declare function pointers to the operators
                unary_operator_fn unaryOp = NULL;
                binary_operator_fn binaryOp = NULL;

                //case for each user input
                switch (op) {
                case '+': binaryOp = operator_add; break;
                case '-': binaryOp = operator_sub; break;
                case '*': binaryOp = operator_mul; break;
                case '/': binaryOp = operator_div; break;
                case '%': binaryOp = fmod; break;
                case 's': unaryOp = sin_function; break;
                case 'c': unaryOp = cos_function; break;
                case 't': unaryOp = tan_function; break;
                case 'a': unaryOp = abs_function; break;
                case 'r': unaryOp = sqrt_function; break;
                }

                //check if there enuf operands on stack
                if (binaryOp) {
                    double rhs, lhs;
                    if (stack_pop(&rhs) && stack_pop(&lhs)) {
                        double result = binaryOp(lhs, rhs); //apply binary opper to opperands
                        stack_push(result); //push back 2 stak
                    } else {
                        printf("Not enough operands on the stack for binary operator: %c\n", op);
                        break;
                    }
                }

                //this is basically doing same but using the cool operators and stuff
                else if (unaryOp) {
                    double operand;
                    if (stack_pop(&operand)) {
                        double result = unaryOp(operand);
                        stack_push(result);
                    } else {
                        printf("Not enough operands on the stack for Unary operator: %c\n", op);
                        break;
                    }
                }

                //handles case of invalid op and the one below does token
                //
                else {
                    printf("Invalid operator: %c\n", op);
                    break;
                }
            } else {
                printf("Invalid token: %s\n", token);
                break;
            }
            token = strtok_r(NULL, " \n", &saveptr);
        }

        // Print the stack after processing the input
        stack_print();
        printf("\n");

        // Clear the stack for the next input
        stack_clear();
    }

    return 0;
}
