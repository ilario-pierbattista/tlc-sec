#include "gtest/gtest.h"

extern "C" {
#include "des/des.h"
}

TEST(des_test, block_list_append) {
    des_block_list_t list_1 = 0, list_2 = 0, list_3 = 0;
    des_block_node_t *node_2, *node_3;
    node_2 = (des_block_node_t *) malloc(sizeof(des_block_node_t));
    node_3 = (des_block_node_t *) malloc(sizeof(des_block_node_t));
    node_2->block.block[0] = 2;
    node_2->next = 0;
    node_3->block.block[0] = 3;
    node_3->next = 0;
    list_2 = node_2;
    list_3 = node_3;

    des_block_list_append(&list_1, list_2);
    EXPECT_EQ(list_1->block.block[0], 2);
    EXPECT_EQ(list_2->block.block[0], 2);

    des_block_list_append(&list_2, list_3);
    EXPECT_EQ(list_2->block.block[0], 2);
    EXPECT_EQ(list_2->next->block.block[0], 3);
    EXPECT_EQ(list_3->block.block[0], 3);

    EXPECT_EQ(list_1->block.block[0], 2);
    EXPECT_EQ(list_1->next->block.block[0], 3);
    EXPECT_EQ((unsigned long long) list_1->next->next, 0);

    free(node_2);
    free(node_3);
}

TEST(des_test, block_list) {
    __uint8_t string[16] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
                            'j', 'k', 'l', 'm', 'n', 'o', 'p'};
    __uint8_t first_expected[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g',
                                   'h'};
    __uint8_t second_expected[8] = {'i', 'j', 'k', 'l', 'm', 'n', 'o',
                                    'p'};
    des_block_list_t list;

    des_generate_block_list(&list, string, 16);

    EXPECT_EQ(memcmp(list->block.block, first_expected, 4), 0);
    EXPECT_EQ(memcmp(list->next->block.block, second_expected, 4), 0);
    EXPECT_EQ((unsigned long long) list->next->next, 0);
}

TEST(des_test, left_key_shift) {
    des_4bytes_t c0, c1_1, c1_2, c1_1_exp, c1_2_exp;
    memset(c0.u8, (__uint8_t) 0x55, 4);
    memset(c1_1_exp.u8, (__uint8_t) 0x2B, 4);
    memset(c1_2_exp.u8, (__uint8_t) 0x56, 4);
    memset(c1_1.u8, (__uint8_t) 0, 4);
    memset(c1_2.u8, (__uint8_t) 0, 4);

    des_left_shift_half_key(&c1_1, &c0, 0);
    EXPECT_EQ(c1_1.u32, c1_1_exp.u32);
    des_left_shift_half_key(&c1_2, &c0, 2);
    EXPECT_EQ(c1_2.u32, c1_2_exp.u32);
}