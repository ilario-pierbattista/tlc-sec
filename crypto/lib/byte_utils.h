//
// Created by ilario on 28/04/17.
//

#ifndef TLC_SEC_BYTE_UTILS_H
#define TLC_SEC_BYTE_UTILS_H

#include <bits/types.h>

int byte_parity_check (__uint8_t byte);

__uint8_t byte_add_parity_bit (__uint8_t byte);

__uint8_t byte_remove_parity_bit (__uint8_t byte);

__uint8_t byte_get_bit (__uint8_t byte, int position);

#endif //TLC_SEC_BYTE_UTILS_H
