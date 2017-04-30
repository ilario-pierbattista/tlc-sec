//
// Created by ilario on 28/04/17.
//

#include <stdio.h>
#include <stdlib.h>
#include <print_utils.h>
#include <memory.h>
#include "des.h"
#include "byte_utils.h"

/***********************************************************************
 *
 * DATA AND S-BOX
 *
 * Refer to https://en.wikipedia.org/wiki/DES_supplementary_material
 ***********************************************************************/

int key_permutation[56] = {57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34,
                           26, 18, 10, 2, 59, 51, 43, 35, 27, 19, 11, 3,
                           60, 52, 44, 36, 63, 55, 47, 39, 31, 23, 15,
                           7, 62, 54, 46, 38, 30, 22, 14, 6, 61, 53, 45,
                           37, 29, 21, 13, 5, 28, 20, 12, 4};

int initial_message_permutation[] = {58, 50, 42, 34, 26, 18, 10, 2, 60,
                                     52, 44, 36, 28, 20, 12, 4, 62, 54,
                                     46, 38, 30, 22, 14, 6, 64, 56, 48,
                                     40, 32, 24, 16, 8, 57, 49, 41, 33,
                                     25, 17, 9, 1, 59, 51, 43, 35, 27,
                                     19, 11, 3, 61, 53, 45, 37, 29, 21,
                                     13, 5, 63, 55, 47, 39, 31, 23, 15,
                                     7};

int key_shift_offsets[17] = {0, 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2,
                             2, 2, 1};

int round_key_filter[48] = {14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10,
                            23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2,
                            41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33,
                            48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36,
                            29, 32};

int message_expansion[48] = {32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9, 8, 9,
                             10, 11, 12, 13, 12, 13, 14, 15, 16, 17, 16,
                             17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
                             24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,
                             1};

int s_boxes[8][64] = {{14, 4,  13, 1,  2,  15, 11, 8,  3,  10, 6,  12, 5,  9,  0,  7,  0,  15, 7,  4,  14, 2,  13, 1,  10, 6, 12, 11, 9,  5,  3,  8, 4,  1,  14, 8,  13, 6,  2,  11, 15, 12, 9,  7,  3,  10, 5,  0,  15, 12, 8,  2,  4,  9,  1,  7,  5,  11, 3,  14, 10, 0, 6,  13},
                      {15, 1,  8,  14, 6,  11, 3,  4,  9,  7,  2,  13, 12, 0,  5,  10, 3,  13, 4,  7,  15, 2,  8,  14, 12, 0, 1,  10, 6,  9,  11, 5, 0,  14, 7,  11, 10, 4,  13, 1,  5,  8,  12, 6,  9,  3,  2,  15, 13, 8,  10, 1,  3,  15, 4,  2,  11, 6,  7,  12, 0,  5, 14, 9},
                      {10, 0,  9,  14, 6,  3,  15, 5,  1,  13, 12, 7,  11, 4,  2,  8,  13, 7,  0,  9,  3,  4,  6,  10, 2,  8, 5,  14, 12, 11, 15, 1, 13, 6,  4,  9,  8,  15, 3,  0,  11, 1,  2,  12, 5,  10, 14, 7,  1,  10, 13, 0,  6,  9,  8,  7,  4,  15, 14, 3,  11, 5, 2,  12},
                      {7,  13, 14, 3,  0,  6,  9,  10, 1,  2,  8,  5,  11, 12, 4,  15, 13, 8,  11, 5,  6,  15, 0,  3,  4,  7, 2,  12, 1,  10, 14, 9, 10, 6,  9,  0,  12, 11, 7,  13, 15, 1,  3,  14, 5,  2,  8,  4,  3,  15, 0,  6,  10, 1,  13, 8,  9,  4,  5,  11, 12, 7, 2,  14},
                      {2,  12, 4,  1,  7,  10, 11, 6,  8,  5,  3,  15, 13, 0,  14, 9,  14, 11, 2,  12, 4,  7,  13, 1,  5,  0, 15, 10, 3,  9,  8,  6, 4,  2,  1,  11, 10, 13, 7,  8,  15, 9,  12, 5,  6,  3,  0,  14, 11, 8,  12, 7,  1,  14, 2,  13, 6,  15, 0,  9,  10, 4, 5,  3},
                      {12, 1,  10, 15, 9,  2,  6,  8,  0,  13, 3,  4,  14, 7,  5,  11, 10, 15, 4,  2,  7,  12, 9,  5,  6,  1, 13, 14, 0,  11, 3,  8, 9,  14, 15, 5,  2,  8,  12, 3,  7,  0,  4,  10, 1,  13, 11, 6,  4,  3,  2,  12, 9,  5,  15, 10, 11, 14, 1,  7,  6,  0, 8,  13},
                      {4,  11, 2,  14, 15, 0,  8,  13, 3,  12, 9,  7,  5,  10, 6,  1,  13, 0,  11, 7,  4,  9,  1,  10, 14, 3, 5,  12, 2,  15, 8,  6, 1,  4,  11, 13, 12, 3,  7,  14, 10, 15, 6,  8,  0,  5,  9,  2,  6,  11, 13, 8,  1,  4,  10, 7,  9,  5,  0,  15, 14, 2, 3,  12},
                      {13, 2,  8,  4,  6,  15, 11, 1,  10, 9,  3,  14, 5,  0,  12, 7,  1,  15, 13, 8,  10, 3,  7,  4,  12, 5, 6,  11, 0,  14, 9,  2, 7,  11, 4,  1,  9,  12, 14, 2,  0,  6,  10, 13, 15, 3,  5,  8,  2,  1,  14, 7,  4,  10, 8,  13, 15, 12, 9,  0,  3,  5, 6,  11}};

int right_sub_message_permutation[32] = {16, 7, 20, 21, 29, 12, 28, 17,
                                         1, 15, 23, 26, 5, 18, 31, 10,
                                         2, 8, 24, 14, 32, 27, 3, 9, 19,
                                         13, 30, 6, 22, 11, 4, 25};

int final_message_permutation[64] = {40, 8, 48, 16, 56, 24, 64, 32, 39,
                                     7, 47, 15, 55, 23, 63, 31, 38, 6,
                                     46, 14, 54, 22, 62, 30, 37, 5, 45,
                                     13, 53, 21, 61, 29, 36, 4, 44, 12,
                                     52, 20, 60, 28, 35, 3, 43, 11, 51,
                                     19, 59, 27, 34, 2, 42, 10, 50, 18,
                                     58, 26, 33, 1, 41, 9, 49, 17, 57,
                                     25};

void des_generate_block_list (des_block_list_t *block_list,
                              __uint8_t *string, size_t size) {
    unsigned short counter = 0;
    des_block_t block_buffer;
    des_block_list_t block_list_tail = 0;
    des_block_node_t *inserting_block = 0;

    *block_list = 0;

    for (int i = 0; i < size; ++i) {
        block_buffer.u8[counter++] = string[i];
        if (counter % 8 == 0) {  // A u8 is completed
            counter = 0;

            // Creating new u8 for the list
            inserting_block = (des_block_node_t *) malloc(
                    sizeof(des_block_node_t));
            memcpy(inserting_block->block.u8, block_buffer.u8, 8);
            inserting_block->next = 0;

            // Appending u8
            if (*block_list != 0) { // General Case
                des_block_list_append(&block_list_tail->next,
                                      inserting_block);
            } else {
                des_block_list_append(block_list, inserting_block);
            }
            block_list_tail = inserting_block;
        }
    }
}

void des_generate_key (char *password, des_key_t *key) {
    __uint8_t ascii_char;

    for (int i = 0; i < 8; ++i) {
        // Mask the byte with 01111111
        ascii_char = ((__uint8_t) 0x7F) & password[i];

        // Add parity bit
        ascii_char = byte_add_parity_bit(ascii_char);

        // Add byte to u8
        key->key[i] = ascii_char;
    }
}

void des_block_list_append (des_block_list_t *base,
                            des_block_list_t append) {
    if (*base == 0) {
        *base = append;
    } else {
        des_block_list_append(&((*base)->next), append);
    }
}

void des_left_shift_half_key (des_4bytes_t *next, des_4bytes_t *current,
                              short round) {
    int shift = key_shift_offsets[round];
    __uint8_t left_byte, right_byte, left_part, right_part;
    next->u32 = 0;

    for (int i = 0; i < 4; ++i) {
        left_byte = byte_remove_parity_bit(current->u8[i]);
        right_byte = byte_remove_parity_bit(current->u8[(i + 1) % 4]);

        switch (shift) {
            case 1: {
                // Masks for 1 bit left shifting
                // xbbb bbbb &      xddd dddd &
                // 0011 1111 =      0100 0000 =
                // 00bb bbbb << 1   0d00 0000 >> 6
                // 0bbb bbb0    |   0000 000d = 0bbb bbbd
                left_part = (left_byte & (__uint8_t) 0x3F) << 1;
                right_part = (right_byte & (__uint8_t) 0x40) >> 6;
                break;
            }
            case 2: {
                // Masks for 2 bits left shifting
                // xbbb bbbb &      xddd dddd &
                // 0001 1111 =      0110 0000 =
                // 000b bbbb << 2   0dd0 0000 >> 5
                // 0bbb bb00    |   0000 00dd = 0bbb bbdd
                left_part = (left_byte & (__uint8_t) 0x1F) << 2;
                right_part = (right_byte & (__uint8_t) 0x60) >> 5;
                break;
            }
            default: {
                left_part = left_byte;
                right_part = (__uint8_t) 0x00;
            }
        }

        next->u8[i] = byte_add_parity_bit(left_part | right_part);
    }
}

void des_key_permutation (des_key_t *out, des_key_t *in) {
    int abs_bit_position, in_byte_number, in_byte_offset;
    char out_byte_number, out_byte_offset;
    __uint8_t extracted_bit;

    memset(out->key, 0, 8);

    out_byte_number = 0;
    out_byte_offset = 0;
    for (int i = 0; i < 56; ++i) {
        abs_bit_position = key_permutation[i];
        in_byte_number = abs_bit_position / 8;
        in_byte_offset = abs_bit_position % 8;

        extracted_bit = byte_get_bit(in->key[in_byte_number],
                                     in_byte_offset);

        out->key[out_byte_number] = out->key[out_byte_number] |
                                    (extracted_bit
                                            << (7 - out_byte_offset));

        if (++out_byte_offset % 7 == 0) {
            out->key[out_byte_number] = byte_add_parity_bit(
                    out->key[out_byte_number] >> 1);
            out_byte_number++;
            out_byte_offset = 0;
        }
    }
}

void
des_filter_56B_key (des_round_key_t *round_key, des_key_t *in_key) {
    int k_bit_num, k_byte_num, k_byte_offset;
    int round_key_byte_num, round_key_byte_offset;
    __uint8_t extracted_bit;
    des_key_t key;

    for (int i = 0; i < 8; ++i) {
        key.key[i] = byte_remove_parity_bit(in_key->key[i]);
    }
    memset(round_key->u8, 0, 6);

    for (int rk_bit_num = 0; rk_bit_num < 48; ++rk_bit_num) {
        k_bit_num = round_key_filter[rk_bit_num] - 1;

        k_byte_num = k_bit_num / 7;
        k_byte_offset = k_bit_num % 7;

        extracted_bit = byte_get_bit(key.key[k_byte_num],
                                     k_byte_offset + 2);

        round_key_byte_num = rk_bit_num / 8;
        round_key_byte_offset = rk_bit_num % 8;

        round_key->u8[round_key_byte_num] |= (extracted_bit
                << (7 - round_key_byte_offset));
    }
}

void
des_generate_round_key (des_round_key_t *round_key, des_key_t *next_key,
                        des_key_t *current_key, short round) {
    memset(round_key->u8, 0x00, 6);

    if (round > 0) {
        des_left_shift_half_key(&next_key->splitted.c,
                                &current_key->splitted.c, round);
        des_left_shift_half_key(&next_key->splitted.d,
                                &current_key->splitted.d, round);
        des_filter_56B_key(round_key, next_key);
    } else {
        des_key_permutation(next_key, current_key);
    }
}

void des_half_block_expansion (des_expanded_block_t *dest,
                               des_half_block_t *half_block) {
    int e_bit_num, e_byte_num, e_byte_offset;
    int h_bit_num, h_byte_num, h_byte_offset;
    __uint8_t extracted_bit;

    memset(dest->u8, 0, 6);

    for (e_bit_num = 0; e_bit_num < 48; ++e_bit_num) {

        e_byte_num = e_bit_num / 8;
        e_byte_offset = e_bit_num % 8;

        h_bit_num = message_expansion[e_bit_num] - 1;
        h_byte_num = h_bit_num / 8;
        h_byte_offset = h_bit_num % 8;

        extracted_bit = byte_get_bit(half_block->u8[h_byte_num],
                                     h_byte_offset + 1)
                << (7 - e_byte_offset);

        dest->u8[e_byte_num] |= extracted_bit;
    }
}

__uint8_t des_sub_box (__uint8_t byte, int s_box_index) {
    return (__uint8_t) (s_boxes[s_box_index][byte]) & (__uint8_t) 0x0F;
}

void des_sub_half_block_permutation (des_half_block_t *dest,
                                     des_half_block_t *src) {
    int d_bit_num, d_byte_num, d_byte_offset;
    int s_bit_num, s_byte_num, s_byte_offset;
    __uint8_t extracted_bit;

    for (d_bit_num = 0; d_bit_num < 32; ++d_bit_num) {

        d_byte_num = d_bit_num / 8;
        d_byte_offset = d_bit_num % 8;

        s_bit_num = right_sub_message_permutation[d_bit_num] - 1;
        s_byte_num = s_bit_num / 8;
        s_byte_offset = s_bit_num % 8;

        extracted_bit =
                byte_get_bit(src->u8[s_byte_num], s_byte_offset + 1)
                        << (7 - d_byte_offset);

        dest->u8[d_byte_num] |= extracted_bit;
    }
}

void des_feistel (des_half_block_t *dest_sub_block,
                  des_half_block_t *src_sub_block,
                  des_round_key_t *round_key) {
    des_expanded_block_t expanded_sub_block;
    des_half_block_t sub_half_block;
    __uint8_t sub_bytes[8];

    des_half_block_expansion(&expanded_sub_block, src_sub_block);
    expanded_sub_block.u16[0] ^= round_key->u16[0];
    expanded_sub_block.u16[1] ^= round_key->u16[1];
    expanded_sub_block.u16[2] ^= round_key->u16[2];

    memset(sub_bytes, 0, 8);
    byte_4packs_of_6bit(&sub_bytes[0], &expanded_sub_block.u8[0]);
    byte_4packs_of_6bit(&sub_bytes[4], &expanded_sub_block.u8[3]);
    sub_half_block.u32 = 0;

    for (int i = 0; i < 8; ++i) {
        if (i % 2 == 0) {
            sub_half_block.u8[i / 2] =
                    des_sub_box(sub_bytes[i], i) << 4;
        } else {
            sub_half_block.u8[i / 2] |= des_sub_box(sub_bytes[i], i);
        }
    }

    des_sub_half_block_permutation(dest_sub_block, &sub_half_block);
}

void des_feistel_round (des_block_t *dest, des_block_t *src,
                        des_round_key_t *round_key) {
    des_half_block_t feistel_result;
    dest->u64 = 0;
    feistel_result.u32 = 0;

    // L(i) = R(i-1)
    dest->splitted.l.u32 = src->splitted.r.u32;
    des_feistel(&feistel_result, &src->splitted.r, round_key);
    // R(i) = f(R(i-1),K(i)) XOR L(i-1)
    dest->splitted.r.u32 = feistel_result.u32 ^ src->splitted.l.u32;
}

void des_generate_keyring (des_round_key_t keyring[16],
                           des_key_t *initial_key) {
    for (int j = 0; j < 16; ++j) {
        memset(keyring[j].u8, 0, 6);
    }

    des_key_t current, next;
    des_round_key_t round_key;

    des_generate_round_key(&round_key, &current, initial_key, 0);
    for (short round = 0; round < 16; round++) {
        des_generate_round_key(&keyring[round], &next, &current,
                               round + (short) 1);
        current.u64 = next.u64;
    }
}

void des_encrypt (des_block_t *cipher_text, des_block_t *plain_text,
                  des_key_t *key) {
    des_block_t block;

    memcpy(&block, plain_text, sizeof(des_block_t));

    for (int i = 0; i < 16; ++i) {
        printf("Round: %d", i + 1);
        printf("Left: \n");
        print_bin(block.splitted.l.u8, 4);
        printf("Right: \n");
        print_bin(block.splitted.r.u8, 4);
    }
}

