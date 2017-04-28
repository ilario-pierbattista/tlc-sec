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

__uint8_t add_parity_bit (__uint8_t byte) {
    if (!byte_parity_check(byte)) {
        return byte | (__uint8_t) 0x80;
    } else {
        return byte;
    }
}

__uint8_t remove_parity_bit (__uint8_t byte) {
    return byte & (__uint8_t) 0x7F;
}