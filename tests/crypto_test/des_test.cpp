#include "gtest/gtest.h"

extern "C" {
#include "des/des.h"
}

TEST(des, block_list_append) {
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

TEST(des, block_list) {
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

TEST(des, left_key_shift) {
    des_4bytes_t c0, c1_1, c1_2, c1_1_exp, c1_2_exp;
    memset(c0.u8, (__uint8_t) 0x55, 4);
    memset(c1_1_exp.u8, (__uint8_t) 0xA9, 4);
    memset(c1_2_exp.u8, (__uint8_t) 0x53, 4);
    memset(c1_1.u8, (__uint8_t) 0, 4);
    memset(c1_2.u8, (__uint8_t) 0, 4);

    des_4bytes_t d0, d1, d1_exp;
    memset(d0.u8, (__uint8_t) 0, 4);
    memset(d1.u8, (__uint8_t) 0, 4);
    memset(d1_exp.u8, (__uint8_t) 0, 4);
    d0.u8[0] = 0x81;
    d1_exp.u8[3] = 0x03;

    des_left_shift_half_key(&c1_1, &c0, 0);
    EXPECT_EQ(c1_1.u32, c0.u32);
    des_left_shift_half_key(&c1_1, &c0, 1);
    EXPECT_EQ(c1_1.u32, c1_1_exp.u32);
    des_left_shift_half_key(&c1_2, &c0, 3);
    EXPECT_EQ(c1_2.u32, c1_2_exp.u32);

    des_left_shift_half_key(&d1, &d0, 1);
    EXPECT_EQ(d1.u32, d1_exp.u32);
}

TEST(des, key_permutation) {
    des_key_t initial_key, c0d0, c0d0_exp;
    memset(initial_key.key, 0, 8);
    memset(c0d0_exp.key, 0, 8);
    initial_key.key[7] = 0x03;
    c0d0_exp.key[4] = 0x81;

    des_key_permutation(&c0d0, &initial_key);
    EXPECT_EQ(memcmp(c0d0.key, c0d0_exp.key, 8), 0);
}

TEST(des, filter_key) {
    des_key_t cd;
    des_round_key_t round_key, expected_round_key;
    memset(cd.key, 0, 8);
    cd.key[7] = 0x03;
    memset(expected_round_key.u8, 0, 6);
    expected_round_key.u8[4] = 0x01;

    des_filter_56B_key(&round_key, &cd);
    EXPECT_EQ(memcmp(round_key.u8, expected_round_key.u8, 6), 0);
}

TEST(des, round_key) {
    des_key_t initial_key, c0d0, c0d0_exp, c1d1, c1d1_exp;
    des_round_key_t round_key, round_key_exp;
    memset(initial_key.key, 0, 8);
    memset(c0d0_exp.key, 0, 8);
    memset(c1d1_exp.key, 0, 8);
    memset(round_key_exp.u8, 0, 6);
    initial_key.key[7] = 0x03;
    c0d0_exp.key[4] = 0x81;
    c1d1_exp.key[7] = 0x03;
    round_key_exp.u8[4] = 0x01;

    des_generate_round_key(&round_key, &c0d0, &initial_key, 0);
    EXPECT_EQ(memcmp(c0d0.key, c0d0_exp.key, 8), 0);

    des_generate_round_key(&round_key, &c1d1, &c0d0, 1);
    EXPECT_EQ(memcmp(c1d1.key, c1d1_exp.key, 8), 0);
    EXPECT_EQ(memcmp(round_key.u8, round_key_exp.u8, 6), 0);
}

TEST(des, block_expansion) {
    des_half_block_t r;
    des_expanded_block_t expanded_block, expanded_block_exp;

    r.u32 = 0;
    r.u8[3] = 0x01;
    memset(expanded_block.u8, 0, 6);
    memset(expanded_block_exp.u8, 0, 6);
    expanded_block_exp.u8[0] = 0x80;
    expanded_block_exp.u8[5] = 0x02;

    des_half_block_expansion(&expanded_block, &r);
    EXPECT_EQ(memcmp(expanded_block.u8, expanded_block_exp.u8, 6), 0);
}

TEST(des, sub_box) {
    EXPECT_EQ(des_sub_box(0x20, 0), 0x04);
    EXPECT_EQ(des_sub_box(0x00, 1), 0x0F);
    EXPECT_EQ(des_sub_box(0x00, 2), 0x0A);
    EXPECT_EQ(des_sub_box(0x00, 3), 0x07);
    EXPECT_EQ(des_sub_box(0x00, 4), 0x02);
    EXPECT_EQ(des_sub_box(0x00, 5), 0x0C);
    EXPECT_EQ(des_sub_box(0x00, 6), 0x04);
    EXPECT_EQ(des_sub_box(0x03, 7), 0x04);
}

TEST(des, sub_half_block_permutation) {
    des_half_block_t before, after, expected;
    memset(before.u8, 0, 4);
    memset(after.u8, 0, 4);
    memset(expected.u8, 0, 4);

    before.u8[0] = 0x4F;
    before.u8[1] = 0xA7;
    before.u8[2] = 0x2C;
    before.u8[3] = 0x44;
    expected.u8[0] = 0xD0;
    expected.u8[1] = 0x58;
    expected.u8[2] = 0xD1;
    expected.u8[3] = 0xBC;

    des_sub_half_block_permutation(&after, &before);
    EXPECT_EQ(after.u32, expected.u32);
}

TEST(des, feistel_func) {
    des_half_block_t before, after, expected;
    des_round_key_t round_key;
    before.u32 = 0;
    after.u32 = 0;
    expected.u32 = 0;
    memset(round_key.u8, 0, 6);

    before.u8[3] = 0x01;
    round_key.u8[5] = 0x01;
    expected.u8[0] = 0xD0;
    expected.u8[1] = 0x58;
    expected.u8[2] = 0xD1;
    expected.u8[3] = 0xBC;

    des_feistel(&after, &before, &round_key);
    EXPECT_EQ(after.u32, expected.u32);
}