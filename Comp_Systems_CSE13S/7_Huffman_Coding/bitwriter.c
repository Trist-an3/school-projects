#include "bitwriter.h"

#include <stdio.h>
#include <stdlib.h>

struct BitWriter {
    FILE *underlying_stream;
    uint8_t byte;
    uint8_t bit_position;
};

BitWriter *bit_write_open(const char *filename) {
    BitWriter *writer = (BitWriter *) calloc(1, sizeof(BitWriter));
    if (writer == NULL) {
        fprintf(stderr, "Error: Allocation failure\n");
        return NULL;
    }

    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        fprintf(stderr, "Error: unable to open file.\n");
        free(writer);
        return NULL;
    }

    writer->underlying_stream = file;
    writer->byte = 0;
    writer->bit_position = 0;
    return writer;
}

void bit_write_close(BitWriter **pbuf) {
    if (pbuf != NULL && *pbuf != NULL) {
        if (((*pbuf)->bit_position) > 0) {
            fputc((*pbuf)->byte, (*pbuf)->underlying_stream);
        }
        fclose((*pbuf)->underlying_stream);
        free(*pbuf);
        *pbuf = NULL;
    }
}

void bit_write_bit(BitWriter *buf, uint8_t bit) {
    if (buf->bit_position > 7) {
        fputc(buf->byte, buf->underlying_stream);
        buf->byte = 0;
        buf->bit_position = 0;
    }

    // buf->byte |= ((bit & 1) << buf->bit_position);
    buf->byte |= (bit << buf->bit_position);

    buf->bit_position++;
}

void bit_write_uint8(BitWriter *buf, uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        bit_write_bit(buf, byte >> i);
    }
}

void bit_write_uint16(BitWriter *buf, uint16_t x) {
    for (int i = 0; i < 16; i++) {
        bit_write_bit(buf, (uint8_t) (x >> i));
    }
}

void bit_write_uint32(BitWriter *buf, uint32_t x) {
    for (int i = 0; i < 32; i++) {
        bit_write_bit(buf, (uint8_t) (x >> i));
    }
}
