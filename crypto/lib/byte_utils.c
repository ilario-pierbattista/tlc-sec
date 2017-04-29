//
// Created by ilario on 28/04/17.
//

#include "byte_utils.h"
#include <memory.h>

int byte_parity_check (__uint8_t byte) {
    short counter = 0;

    for (int i = 0; i < 8; ++i) {
        if (0x01 & byte) {
            counter++;
        }
        byte = byte >> 1;
    }

    return (counter % 2 == 0);
}

__uint8_t byte_add_parity_bit (__uint8_t byte) {
    byte = (byte & (__uint8_t) 0x7F) << 1;

    if (!byte_parity_check(byte)) {
        return byte | (__uint8_t) 0x01;
    } else {
        return byte;
    }
}

__uint8_t byte_remove_parity_bit (__uint8_t byte) {
    return (byte & (__uint8_t) 0xFE) >> 1;
}

__uint8_t byte_get_bit (__uint8_t byte, int position) {
    __uint8_t masks[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02,
                          0x01};

    return (byte & masks[position - 1]) >> (8 - position);
}

void byte_4packs_of_6bit (__uint8_t dest[4], __uint8_t src[3]) {
    memset(dest, 0, 4);

    dest[0] = (src[0] & (__uint8_t) 0xFC) >> 2;

    dest[1] = (src[0] & (__uint8_t) 0x03) << 4;
    dest[1] |= (src[1] & (__uint8_t) 0xF0) >> 4;

    dest[2] = (src[1] & (__uint8_t) 0x0F) << 2;
    dest[2] |= (src[2] & (__uint8_t) 0xC0) >> 6;

    dest[3] = (src[2] & (__uint8_t) 0x3F);
}

void byte_3packs_of_8bit (__uint8_t dest[3], __uint8_t src[4]) {
    memset(dest, 0, 3);

    dest[0] = (src[0] & (__uint8_t) 0x3F) << 2;
    dest[0] |= (src[1] & (__uint8_t) 0x30) >> 4;

    dest[1] = (src[1] & (__uint8_t) 0x0F) << 4;
    dest[1] |= (src[2] & (__uint8_t) 0x3C) >> 2;

    dest[2] = (src[2] & (__uint8_t) 0x03) << 6;
    dest[2] |= (src[3] & (__uint8_t) 0x3F);
}