//
// Created by ilario on 28/04/17.
//

#include <stdio.h>
#include <stdlib.h>
#include <des.h>
#include <print_utils.h>
#include <string.h>

int main (int argc, char **argv) {
    des_key_t key;
    des_block_list_t block_list;

    if (argc != 3) {
        printf("Usage: %s <message> <password>\n", argv[0]);
        exit(1);
    }

    printf("Plaintext: %s\n "
                   "Password: %s\n", argv[1], argv[2]);
    printf("Password binary digest:\n");
    print_bin((__uint8_t *) argv[2], 8);
    des_generate_key(argv[2], &key);
    printf("Key with parity bits digest:\n");
    print_bin(key.key, 8);

    des_generate_block_list(&block_list, (__uint8_t *) argv[1],
                            strlen(argv[1]));

    // @TODO Aggiungere lo step di padding del testo

    printf("Plaintext digest:\n");
    int i = 0;
    for (des_block_list_t p = block_list;
         p != 0 && p->next != 0; p = p->next, i++) {
        printf("Block %02X| ", i);
        print_bin(p->block.block, 8);
    }
}
