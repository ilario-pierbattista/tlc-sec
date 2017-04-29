//
// Created by ilario on 28/04/17.
//

#include "byte_utils.h"

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