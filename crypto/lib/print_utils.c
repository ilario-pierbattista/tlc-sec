//
// Created by ilario on 28/04/17.
//

#include <stdio.h>
#include "print_utils.h"

char binary_lookup_table[16][9] = {"0000", "0001", "0010", "0011",
                                   "0100", "0101", "0110", "0111",
                                   "1000", "1001", "1010", "1011",
                                   "1100", "1101", "1110", "1111"};

void print_hex (const __uint8_t *bytes, size_t size) {
    for (int i = 0; i < size; ++i) {
        printf("%02X ", bytes[i]);
    }
    printf("\n");
}

void print_bin (const __uint8_t *bytes, size_t size) {
    for (int i = 0; i < size; ++i) {
        printf("%s%s ", binary_lookup_table[(bytes[i] & 0xF0) >> 4],
               binary_lookup_table[bytes[i] & 0x0F]);
    }
    printf("\n");
}