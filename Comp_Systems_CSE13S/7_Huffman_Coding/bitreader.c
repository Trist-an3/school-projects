#include "bitreader.h"

#include <stdio.h>
#include <stdlib.h>

struct BitReader {
    FILE *underlying_stream;
    uint8_t byte;
    uint8_t bit_position;
};

BitReader *bit_read_open(const char *filename) {
    BitReader *reader = (BitReader *) calloc(1, sizeof(BitReader));
    if (reader == NULL) {
        fprintf(stderr, "Error: Allocation faliure\n");
        return NULL;
    }

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open file %s\n", filename);
        free(reader);
        return NULL;
    }

    reader->underlying_stream = file;
    reader->byte = 0;
    reader->bit_position = 8;
    return reader;
}

void bit_read_close(BitReader **pbuf) {

    if (pbuf != NULL && *pbuf != NULL) {
        fclose((*pbuf)->underlying_stream);
        free(*pbuf);
        *pbuf = NULL;
    } else {
        fprintf(stderr, "Error: Invalid pointer or already closed.\n");
    }
}

uint8_t bit_read_bit(BitReader *buf) {
    if (buf->bit_position > 7) {
        int read_byte = fgetc(buf->underlying_stream);

        if (read_byte == EOF) {
            fprintf(stderr, "Error: Unable to read byte from stream.\n");
            return 1;
        }

        buf->byte = (uint8_t) read_byte;
        buf->bit_position = 0;
    }

    uint8_t bit = (buf->byte >> buf->bit_position) & 1;
    buf->bit_position++;
    return bit;
}

uint8_t bit_read_uint8(BitReader *buf) {
    uint8_t byte = 0x00;
    for (int i = 0; i < 8; i++) {
        uint8_t bit = bit_read_bit(buf);
        byte |= (bit << i);
    }

    return byte;
}

uint16_t bit_read_uint16(BitReader *buf) {
    uint16_t word = 0x0000;
    for (int i = 0; i < 16; i++) {
        uint16_t bit = bit_read_bit(buf);
        word |= (bit << i);
    }

    return word;
}

uint32_t bit_read_uint32(BitReader *buf) {
    uint32_t word = 0x00000000;
    for (int i = 0; i < 32; i++) {
        uint32_t bit = bit_read_bit(buf);
        word |= (bit << i);
    }

    return word;
}
