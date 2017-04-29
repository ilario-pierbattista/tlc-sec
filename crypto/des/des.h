//
// Created by ilario on 28/04/17.
//

#ifndef TLC_SEC_DES_H
#define TLC_SEC_DES_H

union des_4B {
    __uint8_t u8[4];
    __uint32_t u32;
};
typedef union des_4B des_4bytes_t;

// DES key stucture w/ parity check bits
union des_key {
    __uint8_t key[8];   // Each byte has a parity check bit
    struct {
        des_4bytes_t c;
        des_4bytes_t d;
    } splitted;
};
typedef union des_key des_key_t;

union des_round_key {
    __uint8_t key[6];   // 48 bit key
};
typedef union des_round_key des_round_key_t;

// Block of plaintext/ciphertext handled by DES
union des_block {
    __uint8_t block[8];
    struct {
        des_4bytes_t l;
        des_4bytes_t r;
    } splitted;
};
typedef union des_block des_block_t;

// List of DES blocks
struct des_block_node {
    des_block_t block;
    struct des_block_node *next;
};
typedef struct des_block_node des_block_node_t;
typedef struct des_block_node *des_block_list_t;

void des_generate_block_list (des_block_list_t *block_list,
                              __uint8_t *string, size_t size);

void
des_block_list_append (des_block_list_t *base, des_block_list_t append);

void des_generate_key (char *password, des_key_t *key);

void des_encrypt (des_block_t *cipher_text, des_block_t *plain_text,
                  des_key_t *key);

void
des_feistel (des_4bytes_t *dest_sub_block, des_4bytes_t *src_sub_block,
             des_round_key_t *round_key);

void
des_generate_round_key (des_round_key_t *round_key, des_key_t *next_key,
                        des_key_t *current_key, short round);

void des_left_shift_half_key (des_4bytes_t *next, des_4bytes_t *current,
                              short round);

void des_filter_56B_key (des_round_key_t *round_key, des_key_t *key);

void des_key_permutation(des_key_t *out, des_key_t *in);

#endif //TLC_SEC_DES_H
