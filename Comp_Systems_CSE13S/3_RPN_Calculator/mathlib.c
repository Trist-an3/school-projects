/*Functions from mathlib.h flile are defined here
 * this file pretty much has all the calculations for the math things
 */

#include "mathlib.h"

#include <math.h>

//Compute the absolute value of x.
double Abs(double x) {

    if (x < 0) {
        return -x;
    } else {
        return x;
    }
}

//Compute the square root of x >= 0.
double Sqrt(double x) {
    //Check domain
    if (x < 0) {
        return nan("nan");
    }

    double old = 0.0;
    double new = 1.0;

    while (Abs(old - new) > EPSILON) {
        //Using babylonian method
        //newton's method possible for sqrt(x)
        old = new;
        new = 0.5 * (old + (x / old));
    }

    return new;
}

//Compute the sine of x.
double Sin(double x) {

    while (x > 2 * M_PI) {
        x -= 2 * M_PI;
    }

    while (x < 0) {
        x += 2 * M_PI;
    }

    double sum = 0;
    double term = x;
    double i = 3;

    while (fabs(term) > EPSILON) {
        sum += term;
        term = -term * x * x / (i * (i - 1));
        i += 2;
    }

    return sum;
}

//Compute the cosine of x.
double Cos(double x) {

    while (x > 2 * M_PI) {
        x -= 2 * M_PI;
    }

    while (x < 0) {
        x += 2 * M_PI;
    }

    double sum = 0;
    double term = 1;
    double i = 2;

    while (fabs(term) > EPSILON) {
        sum += term;
        term = -term * x * x / (i * (i - 1));

        i += 2;
    }

    return sum;
}

/*
 * Compute the tangent of x.
 *
 * NOTE:
 * Tan(M_PI / 2) will not be undefined for two reasons:
 * (1) the IEEE 754 double precision standard cannot perfectly represent
 *     pi in binary; and
 * (2) the Taylor-Maclaurin series used to approximate Cos(pi/2) will not
 *     converge to 0.
 *  Therefore, it will instead to converge to a very, very small
 *  number resulting in a very, very large value of Tan(pi/2).
 */

double Tan(double x) {

    // Normalize the angle to the range [0, 2π]
    while (x > 2 * M_PI) {
        x -= 2 * M_PI;
    }
    while (x < 0) {
        x += 2 * M_PI;
    }

    // Calculate the tangent using the formula Tan(x) = Sin(x) / Cos(x)
    //my functions ofc :)
    double sin_x = Sin(x);
    double cos_x = Cos(x);

    return sin_x / cos_x;
}
