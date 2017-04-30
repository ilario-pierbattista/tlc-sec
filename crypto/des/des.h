#ifndef TLC_SEC_DES_H
#define TLC_SEC_DES_H

#include <ctype.h>
#include <stdlib.h>

/***********************************************************************
 *
 *  TYPE DEFINITIONS
 *
 **********************************************************************/
union des_4B {
    __uint8_t u8[4];
    __uint32_t u32;
};
typedef union des_4B des_4bytes_t;
typedef union des_4B des_half_block_t;

// DES u8 stucture w/ parity check bits
union des_key {
    __uint8_t key[8];   // Each byte has a parity check bit
    struct {
        des_4bytes_t c;
        des_4bytes_t d;
    } splitted;
    __uint64_t u64;
};
typedef union des_key des_key_t;

union des_48B_block {   // 48 bit
    __uint8_t u8[6];
    __uint16_t u16[3];
};
typedef union des_48B_block des_round_key_t;
typedef union des_48B_block des_expanded_block_t;

// Block of plaintext/ciphertext handled by DES
union des_block {
    __uint8_t u8[8];
    __uint64_t u64;
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

/***********************************************************************
 *
 * FUNCTION DECLARATIONS
 *
 **********************************************************************/

/**
 * It generates a list of blocks which can be used by the DES algorithm.
 *
 * @param block_list Pointer to the head of the resulting block list.
 * Resulting blocks are chained togheter after the first pointer.
 * @param string Binary string to slice into blocks. Padding
 * operations are supposed to be done yet. Exceding bytes are ignored.
 * @param size Lenght of the binary string.
 */
void des_generate_block_list (des_block_list_t *block_list,
                              __uint8_t *string, size_t size);

/**
 * It appends a list of blocks to another list.
 *
 * @param base Head of the first list.
 * @param append Head of the second list.
 */
void
des_block_list_append (des_block_list_t *base, des_block_list_t append);

void des_generate_key (char *password, des_key_t *key);

void des_encrypt (des_block_t *cipher_text, des_block_t *plain_text,
                  des_key_t *key);

void des_half_block_expansion (des_expanded_block_t *dest,
                               des_half_block_t *half_block);

__uint8_t des_sub_box (__uint8_t byte, int s_box_index);

void des_sub_half_block_permutation (des_half_block_t *dest,
                                     des_half_block_t *src);

void des_feistel (des_half_block_t *dest_sub_block,
                  des_half_block_t *src_sub_block,
                  des_round_key_t *round_key);

void des_feistel_round (des_block_t *dest, des_block_t *src,
                        des_round_key_t *round_key);

void des_generate_keyring (des_round_key_t keyring[16],
                           des_key_t *initial_key);

void
des_generate_round_key (des_round_key_t *round_key, des_key_t *next_key,
                        des_key_t *current_key, short round);

void des_left_shift_half_key (des_4bytes_t *next, des_4bytes_t *current,
                              short round);

void des_filter_56B_key (des_round_key_t *round_key, des_key_t *key);

void des_key_permutation (des_key_t *out, des_key_t *in);

#endif //TLC_SEC_DES_H
