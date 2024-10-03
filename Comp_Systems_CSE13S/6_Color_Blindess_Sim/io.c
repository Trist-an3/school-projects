//Tristan Garcia
//File is for all the reading and writing options for user input and  output
//source file for the io.h header functoins

#include "io.h"

void read_uint8(FILE *fin, uint8_t *px) {

    //reading each char from the file
    int result = fgetc(fin);

    //checks if result is EOF
    if (result == EOF) {
        fprintf(stderr, "Error: Unexpected end of file while reading in read_uint8()\n");
    }

    *px = (uint8_t) result;
}

void read_uint16(FILE *fin, uint16_t *px) {
    uint16_t result = 0;
    uint8_t b1 = 0, b2 = 0;

    //call read functions to store the values of b1 and b2
    read_uint8(fin, &b1);
    read_uint8(fin, &b2);

    //storing b1 into result and letting program know that its a 16 bit(used a type cast)
    result = (uint16_t) b2;

    //moves b1 to the left 8 bits
    result <<= 8;

    //uses or bitwise
    result |= (uint16_t) b1;

    //return pointer with result
    *px = result;
}

void read_uint32(FILE *fin, uint32_t *px) {
    uint32_t result = 0;
    uint16_t b1 = 0, b2 = 0;

    read_uint16(fin, &b1);
    read_uint16(fin, &b2);

    result = (uint32_t) b2;

    result <<= 16;

    result |= (uint32_t) b1;

    *px = result;
}

void write_uint8(FILE *fout, uint8_t x) {

    //outputing each char to the specified file stream
    int result = fputc(x, fout);

    if (result == EOF) {
        fprintf(stderr, "Error: Unexpected end of file while reading in write_uint8()\n");
    }
}

void write_uint16(FILE *fout, uint16_t x) {

    // Write the lower 8 bits of x to the specified file stream
    write_uint8(fout, (uint8_t) x);

    // Write the upper 8 bits of x to the specified file stream
    write_uint8(fout, x >> 8);
}

void write_uint32(FILE *fout, uint32_t x) {

    write_uint16(fout, (uint16_t) x);
    write_uint16(fout, x >> 16);
}
