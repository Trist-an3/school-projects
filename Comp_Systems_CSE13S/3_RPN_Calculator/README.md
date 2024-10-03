## Programming Assignment 3
* Name: Tristan Garcia
* CruzID: tgarcia7
* StudentID: 1991617

## Purpose:
    This project involves building a Reverse Polish Notation (RPN) calculator. 
    It focuses on implementing a stack-based calculator that evaluates mathematical 
    expressions using function pointers functions without using math.h

## Files included
    Exectuable Files:
    calc.c - Contains the main logic for reading user input and evaluating expressions in RPN.

    Source Files:
    stack.c - Implements the stack ADT used to store operands for the RPN calculator.
    operators.c - Contains implementations of basic math operators and helper functions for unary and binary operations.
    mathlib.c - Implements the sine, cosine, and tangent functions, along with other mathematical utilities.

    Helper file:
    stack.h: Declares the stack ADT function prototypes.
    operators.h: Declares operator functions and sets up function pointers for RPN evaluation.
    mathlib.h: Defines math functions such as sine, cosine, and tangent, implemented without the standard math library.
    messages.h: Contains error and help messages for the program.

    Extras:
    Makefile - Compiles and creates executables
