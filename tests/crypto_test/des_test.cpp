#include "gtest/gtest.h"

extern "C" {
#include "des/des.h"
}

TEST(des, key_generation) {
    char password[] = {'h', 'e', 'l', 'l', 'o', 'd', 'e', 's'};
    des_key_t key, key_expected;
    key_expected.key[0] = 0xD1;
    key_expected.key[1] = 0xCA;
    key_expected.key[2] = 0xD8;
    key_expected.key[3] = 0xD8;
    key_expected.key[4] = 0xDE;
    key_expected.key[5] = 0xC9;
    key_expected.key[6] = 0xCA;
    key_expected.key[7] = 0xE7;

    des_generate_key(password, &key);
    EXPECT_EQ(key.u64, key_expected.u64);
}

TEST(des, block_list_append) {
    des_block_list_t list_1 = 0, list_2 = 0, list_3 = 0;
    des_block_node_t *node_2, *node_3;
    node_2 = (des_block_node_t *) malloc(sizeof(des_block_node_t));
    node_3 = (des_block_node_t *) malloc(sizeof(des_block_node_t));
    node_2->block.u8[0] = 2;
    node_2->next = 0;
    node_3->block.u8[0] = 3;
    node_3->next = 0;
    list_2 = node_2;
    list_3 = node_3;

    des_block_list_append(&list_1, list_2);
    EXPECT_EQ(list_1->block.u8[0], 2);
    EXPECT_EQ(list_2->block.u8[0], 2);

    des_block_list_append(&list_2, list_3);
    EXPECT_EQ(list_2->block.u8[0], 2);
    EXPECT_EQ(list_2->next->block.u8[0], 3);
    EXPECT_EQ(list_3->block.u8[0], 3);

    EXPECT_EQ(list_1->block.u8[0], 2);
    EXPECT_EQ(list_1->next->block.u8[0], 3);
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

    EXPECT_EQ(memcmp(list->block.u8, first_expected, 4), 0);
    EXPECT_EQ(memcmp(list->next->block.u8, second_expected, 4), 0);
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

    // Round 2: shift 1
    // C1 = 0x00 0x00 0x00 0x00
    // D1 = 0x00 0x00 0x00 0x03
    c0d0.u64 = 0;
    c0d0.key[7] = 0x03;
    // C2 = 0x00 0x00 0x00 0x00
    // D2 = 0x00 0x00 0x00 0x05
    c1d1_exp.u64 = 0;
    c1d1_exp.key[7] = 0x05;
    // K2 = 0x00 0x00 0x00 0x04 0x00 0x00
    memset(round_key_exp.u8, 0, 6);
    round_key_exp.u8[3] = 0x04;
    // Test:
    des_generate_round_key(&round_key, &c1d1, &c0d0, 2);
    EXPECT_EQ(c1d1.u64, c1d1_exp.u64);
    EXPECT_EQ(memcmp(round_key.u8, round_key_exp.u8, 6), 0);

    // Round 3: shift 2
    // C2 = 0x00 0x00 0x00 0x00
    // D2 = 0x00 0x00 0x00 0x05
    c0d0.u64 = 0;
    c0d0.key[7] = 0x05;
    // C3 = 0x00 0x00 0x00 0x00
    // D3 = 0x00 0x00 0x00 0x11
    c1d1_exp.u64 = 0;
    c1d1_exp.key[7] = 0x11;
    // K3 = 0x00 0x00 0x00 0x00 0x00 0x40
    memset(round_key_exp.u8, 0, 6);
    round_key_exp.u8[5] = 0x40;
    // Test:
    des_generate_round_key(&round_key, &c1d1, &c0d0, 3);
    EXPECT_EQ(c1d1.u64, c1d1_exp.u64);
    EXPECT_EQ(memcmp(round_key.u8, round_key_exp.u8, 6), 0);

    // Round 4: shift 2
    // C3 = 0x00 0x00 0x00 0x00
    // D3 = 0x00 0x00 0x00 0x11
    c0d0.u64 = 0;
    c0d0.key[7] = 0x11;
    // C4 = 0x00 0x00 0x00 0x00
    // D4 = 0x00 0x00 0x00 0x41
    c1d1_exp.u64 = 0;
    c1d1_exp.key[7] = 0x41;
    // K4 = 0x00 0x00 0x00 0x00 0x80 0x00
    memset(round_key_exp.u8, 0, 6);
    round_key_exp.u8[4] = 0x80;
    // Test:
    des_generate_round_key(&round_key, &c1d1, &c0d0, 4);
    EXPECT_EQ(c1d1.u64, c1d1_exp.u64);
    EXPECT_EQ(memcmp(round_key.u8, round_key_exp.u8, 6), 0);

    // Round 5: shift 2
    // C4 = 0x00 0x00 0x00 0x00
    // D4 = 0x00 0x00 0x00 0x41
    c0d0.u64 = 0;
    c0d0.key[7] = 0x41;
    // C5 = 0x00 0x00 0x00 0x00
    // D5 = 0x00 0x00 0x03 0x00
    c1d1_exp.u64 = 0;
    c1d1_exp.key[6] = 0x03;
    // K5 = 0x00 0x00 0x00 0x00 0x04 0x00
    memset(round_key_exp.u8, 0, 6);
    round_key_exp.u8[4] = 0x04;
    // Test:
    des_generate_round_key(&round_key, &c1d1, &c0d0, 5);
    EXPECT_EQ(c1d1.u64, c1d1_exp.u64);
    EXPECT_EQ(memcmp(round_key.u8, round_key_exp.u8, 6), 0);

    // Round 6: shift 2
    // C5 = 0x00 0x00 0x00 0x00
    // D5 = 0x00 0x00 0x03 0x00
    c0d0.u64 = 0;
    c0d0.key[6] = 0x03;
    // C6 = 0x00 0x00 0x00 0x00
    // D6 = 0x00 0x00 0x09 0x00
    c1d1_exp.u64 = 0;
    c1d1_exp.key[6] = 0x09;
    // K6 = 0x00 0x00 0x00 0x08 0x00 0x00
    memset(round_key_exp.u8, 0, 6);
    round_key_exp.u8[3] = 0x08;
    // Test:
    des_generate_round_key(&round_key, &c1d1, &c0d0, 6);
    EXPECT_EQ(c1d1.u64, c1d1_exp.u64);
    EXPECT_EQ(memcmp(round_key.u8, round_key_exp.u8, 6), 0);

    // Round 7: shift 2
    // C6 = 0x00 0x00 0x00 0x00
    // D6 = 0x00 0x00 0x09 0x00
    c0d0.u64 = 0;
    c0d0.key[6] = 0x09;
    // C7 = 0x00 0x00 0x00 0x00
    // D7 = 0x00 0x00 0x21 0x00
    c1d1_exp.u64 = 0;
    c1d1_exp.key[6] = 0x21;
    // K7 = 0x00 0x00 0x00 0x00 0x40 0x00
    memset(round_key_exp.u8, 0, 6);
    round_key_exp.u8[4] = 0x40;
    // Test:
    des_generate_round_key(&round_key, &c1d1, &c0d0, 7);
    EXPECT_EQ(c1d1.u64, c1d1_exp.u64);
    EXPECT_EQ(memcmp(round_key.u8, round_key_exp.u8, 6), 0);

    // Round 8: shift 2
    // C7 = 0x00 0x00 0x00 0x00
    // D7 = 0x00 0x00 0x21 0x00
    c0d0.u64 = 0;
    c0d0.key[6] = 0x21;
    // C8 = 0x00 0x00 0x00 0x00
    // D8 = 0x00 0x00 0x81 0x00
    c1d1_exp.u64 = 0;
    c1d1_exp.key[6] = 0x81;
    // K8 = 0x00 0x00 0x00 0x00 0x00 0x00
    memset(round_key_exp.u8, 0, 6);
    // Test:
    des_generate_round_key(&round_key, &c1d1, &c0d0, 8);
    EXPECT_EQ(c1d1.u64, c1d1_exp.u64);
    EXPECT_EQ(memcmp(round_key.u8, round_key_exp.u8, 6), 0);

    // Round 9: shift 1
    // C8 = 0x00 0x00 0x00 0x00
    // D8 = 0x00 0x00 0x81 0x00
    c0d0.u64 = 0;
    c0d0.key[6] = 0x81;
    // C9 = 0x00 0x00 0x00 0x00
    // D9 = 0x00 0x03 0x00 0x00
    c1d1_exp.u64 = 0;
    c1d1_exp.key[5] = 0x03;
    // K9 = 0x00 0x00 0x00 0x00 0x00 0x10
    memset(round_key_exp.u8, 0, 6);
    round_key_exp.u8[5] = 0x10;
    // Test:
    des_generate_round_key(&round_key, &c1d1, &c0d0, 9);
    EXPECT_EQ(c1d1.u64, c1d1_exp.u64);
    EXPECT_EQ(memcmp(round_key.u8, round_key_exp.u8, 6), 0);

    // Round 10: shift 2
    // C9 = 0x00 0x00 0x00 0x00
    // D9 = 0x00 0x03 0x00 0x00
    c0d0.u64 = 0;
    c0d0.key[5] = 0x03;
    // C10 = 0x00 0x00 0x00 0x00
    // D10 = 0x00 0x09 0x00 0x00
    c1d1_exp.u64 = 0;
    c1d1_exp.key[5] = 0x09;
    // K10 = 0x00 0x00 0x00 0x01 0x00 0x00
    memset(round_key_exp.u8, 0, 6);
    round_key_exp.u8[3] = 0x01;
    // Test:
    des_generate_round_key(&round_key, &c1d1, &c0d0, 10);
    EXPECT_EQ(c1d1.u64, c1d1_exp.u64);
    EXPECT_EQ(memcmp(round_key.u8, round_key_exp.u8, 6), 0);

    // Round 11: shift 2
    // C10 = 0x00 0x00 0x00 0x00
    // D10 = 0x00 0x09 0x00 0x00
    c0d0.u64 = 0;
    c0d0.key[5] = 0x09;
    // C11 = 0x00 0x00 0x00 0x00
    // D11 = 0x00 0x21 0x00 0x00
    c1d1_exp.u64 = 0;
    c1d1_exp.key[5] = 0x21;
    // K11 = 0x00 0x00 0x00 0x00 0x00 0x00
    memset(round_key_exp.u8, 0, 6);
    // Test:
    des_generate_round_key(&round_key, &c1d1, &c0d0, 11);
    EXPECT_EQ(c1d1.u64, c1d1_exp.u64);
    EXPECT_EQ(memcmp(round_key.u8, round_key_exp.u8, 6), 0);

    // Round 12: shift 2
    // C11 = 0x00 0x00 0x00 0x00
    // D11 = 0x00 0x21 0x00 0x00
    c0d0.u64 = 0;
    c0d0.key[5] = 0x21;
    // C12 = 0x00 0x00 0x00 0x00
    // D12 = 0x00 0x81 0x00 0x00
    c1d1_exp.u64 = 0;
    c1d1_exp.key[5] = 0x81;
    // K12 = 0x00 0x00 0x00 0x00 0x00 0x04
    memset(round_key_exp.u8, 0, 6);
    round_key_exp.u8[5] = 0x04;
    // Test:
    des_generate_round_key(&round_key, &c1d1, &c0d0, 12);
    EXPECT_EQ(c1d1.u64, c1d1_exp.u64);
    EXPECT_EQ(memcmp(round_key.u8, round_key_exp.u8, 6), 0);

    // Round 13: shift 2
    // C12 = 0x00 0x00 0x00 0x00
    // D12 = 0x00 0x81 0x00 0x00
    c0d0.u64 = 0;
    c0d0.key[5] = 0x81;
    // C13 = 0x00 0x00 0x00 0x00
    // D13 = 0x05 0x00 0x00 0x00
    c1d1_exp.u64 = 0;
    c1d1_exp.key[4] = 0x05;
    // K13 = 0x00 0x00 0x00 0x00 0x00 0x80
    memset(round_key_exp.u8, 0, 6);
    round_key_exp.u8[5] = 0x80;
    // Test:
    des_generate_round_key(&round_key, &c1d1, &c0d0, 13);
    EXPECT_EQ(c1d1.u64, c1d1_exp.u64);
    EXPECT_EQ(memcmp(round_key.u8, round_key_exp.u8, 6), 0);

    // Round 14: shift 2
    // C13 = 0x00 0x00 0x00 0x00
    // D13 = 0x05 0x00 0x00 0x00
    c0d0.u64 = 0;
    c0d0.key[4] = 0x05;
    // C14 = 0x00 0x00 0x00 0x00
    // D14 = 0x11 0x00 0x00 0x00
    c1d1_exp.u64 = 0;
    c1d1_exp.key[4] = 0x11;
    // K14 = 0x00 0x00 0x00 0x00 0x00 0x01
    memset(round_key_exp.u8, 0, 6);
    round_key_exp.u8[5] = 0x01;
    // Test:
    des_generate_round_key(&round_key, &c1d1, &c0d0, 14);
    EXPECT_EQ(c1d1.u64, c1d1_exp.u64);
    EXPECT_EQ(memcmp(round_key.u8, round_key_exp.u8, 6), 0);

    // Round 15: shift 2
    // C14 = 0x00 0x00 0x00 0x00
    // D14 = 0x11 0x00 0x00 0x00
    c0d0.u64 = 0;
    c0d0.key[4] = 0x11;
    // C15 = 0x00 0x00 0x00 0x00
    // D15 = 0x41 0x00 0x00 0x00
    c1d1_exp.u64 = 0;
    c1d1_exp.key[4] = 0x41;
    // K15 = 0x00 0x00 0x00 0x02 0x00 0x00
    memset(round_key_exp.u8, 0, 6);
    round_key_exp.u8[3] = 0x02;
    // Test:
    des_generate_round_key(&round_key, &c1d1, &c0d0, 15);
    EXPECT_EQ(c1d1.u64, c1d1_exp.u64);
    EXPECT_EQ(memcmp(round_key.u8, round_key_exp.u8, 6), 0);

    // Round 16: shift 1
    // C15 = 0x00 0x00 0x00 0x00
    // D15 = 0x41 0x00 0x00 0x00
    c0d0.u64 = 0;
    c0d0.key[4] = 0x41;
    // C16 = 0x00 0x00 0x00 0x00
    // D16 = 0x81 0x00 0x00 0x00
    c1d1_exp.u64 = 0;
    c1d1_exp.key[4] = 0x81;
    // K16 = 0x00 0x00 0x00 0x00 0x00 0x02
    memset(round_key_exp.u8, 0, 6);
    round_key_exp.u8[5] = 0x02;
    // Test:
    des_generate_round_key(&round_key, &c1d1, &c0d0, 16);
    EXPECT_EQ(c1d1.u64, c1d1_exp.u64);
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

TEST(des, feistel_round) {
    des_block_t before, after, expected;
    des_round_key_t round_key;
    before.u64 = 0;
    after.u64 = 0;
    expected.u64 = 0;
    memset(round_key.u8, 0, 6);

    // L0 = 0x00 0x00 0x00 0x01
    before.splitted.l.u8[3] = 0x01;
    // R0 = 0x00 0x00 0x00 0x01
    before.splitted.r.u8[3] = 0x01;
    // K0 = 0x00 0x00 0x00 0x00 0x01
    round_key.u8[5] = 0x01;

    // L1 = 0x00 0x00 0x00 0x01
    expected.splitted.l.u8[3] = 0x01;
    // R1 = 0xD0 0x58 0xD1 0xBD
    expected.splitted.r.u8[0] = 0xD0;
    expected.splitted.r.u8[1] = 0x58;
    expected.splitted.r.u8[2] = 0xD1;
    expected.splitted.r.u8[3] = 0xBD;

    des_feistel_round(&after, &before, &round_key);
    EXPECT_EQ(after.u64, expected.u64);
}

TEST(des, encryption_keyring) {
    des_key_t initial_key;
    des_round_key_t keyring[16], keyring_expected[16];
    initial_key.u64 = 0;
    for (int j = 0; j < 16; ++j) {
        memset(keyring[j].u8, 0, 6);
        memset(keyring_expected[j].u8, 0, 6);
    }

    // K0 = 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x03
    initial_key.key[7] = 0x03;

    // K1 = 0x00 0x00 0x00 0x00 0x01 0x00
    keyring_expected[0].u8[4] = 0x01;
    // K2 = 0x00 0x00 0x00 0x04 0x00 0x00
    keyring_expected[1].u8[3] = 0x04;
    // K3 = 0x00 0x00 0x00 0x00 0x00 0x40
    keyring_expected[2].u8[5] = 0x40;
    // K4 = 0x00 0x00 0x00 0x00 0x80 0x00
    keyring_expected[3].u8[4] = 0x80;
    // K5 = 0x00 0x00 0x00 0x00 0x04 0x00
    keyring_expected[4].u8[4] = 0x04;
    // K6 = 0x00 0x00 0x00 0x08 0x00 0x00
    keyring_expected[5].u8[3] = 0x08;
    // K7 = 0x00 0x00 0x00 0x00 0x40 0x00
    keyring_expected[6].u8[4] = 0x40;
    // K8 = 0x00 0x00 0x00 0x00 0x00 0x00

    // K9 = 0x00 0x00 0x00 0x00 0x00 0x10
    keyring_expected[8].u8[5] = 0x10;
    // K10 = 0x00 0x00 0x00 0x01 0x00 0x00
    keyring_expected[9].u8[3] = 0x01;
    // K11 = 0x00 0x00 0x00 0x00 0x00 0x00

    // K12 = 0x00 0x00 0x00 0x00 0x00 0x04
    keyring_expected[11].u8[5] = 0x04;
    // K13 = 0x00 0x00 0x00 0x00 0x00 0x80
    keyring_expected[12].u8[5] = 0x80;
    // K14 = 0x00 0x00 0x00 0x00 0x00 0x01
    keyring_expected[13].u8[5] = 0x01;
    // K15 = 0x00 0x00 0x00 0x02 0x00 0x00
    keyring_expected[14].u8[3] = 0x02;
    // K16 = 0x00 0x00 0x00 0x00 0x00 0x02
    keyring_expected[15].u8[5] = 0x02;

    des_generate_encryption_keyring(keyring, &initial_key);
    for (int i = 0; i < 16; ++i) {
        EXPECT_EQ(memcmp(keyring[i].u8, keyring_expected[i].u8, 6), 0);
    }
}

TEST(des, decryption_keyring) {
    des_key_t initial_key;
    des_round_key_t keyring[16], keyring_expected[16];
    initial_key.u64 = 0;
    for (int j = 0; j < 16; ++j) {
        memset(keyring[j].u8, 0, 6);
        memset(keyring_expected[j].u8, 0, 6);
    }

    // K0 = 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x03
    initial_key.key[7] = 0x03;

    // K1 = 0x00 0x00 0x00 0x00 0x01 0x00
    keyring_expected[15].u8[4] = 0x01;
    // K2 = 0x00 0x00 0x00 0x04 0x00 0x00
    keyring_expected[14].u8[3] = 0x04;
    // K3 = 0x00 0x00 0x00 0x00 0x00 0x40
    keyring_expected[13].u8[5] = 0x40;
    // K4 = 0x00 0x00 0x00 0x00 0x80 0x00
    keyring_expected[12].u8[4] = 0x80;
    // K5 = 0x00 0x00 0x00 0x00 0x04 0x00
    keyring_expected[11].u8[4] = 0x04;
    // K6 = 0x00 0x00 0x00 0x08 0x00 0x00
    keyring_expected[10].u8[3] = 0x08;
    // K7 = 0x00 0x00 0x00 0x00 0x40 0x00
    keyring_expected[9].u8[4] = 0x40;
    // K8 = 0x00 0x00 0x00 0x00 0x00 0x00

    // K9 = 0x00 0x00 0x00 0x00 0x00 0x10
    keyring_expected[7].u8[5] = 0x10;
    // K10 = 0x00 0x00 0x00 0x01 0x00 0x00
    keyring_expected[6].u8[3] = 0x01;
    // K11 = 0x00 0x00 0x00 0x00 0x00 0x00

    // K12 = 0x00 0x00 0x00 0x00 0x00 0x04
    keyring_expected[4].u8[5] = 0x04;
    // K13 = 0x00 0x00 0x00 0x00 0x00 0x80
    keyring_expected[3].u8[5] = 0x80;
    // K14 = 0x00 0x00 0x00 0x00 0x00 0x01
    keyring_expected[2].u8[5] = 0x01;
    // K15 = 0x00 0x00 0x00 0x02 0x00 0x00
    keyring_expected[1].u8[3] = 0x02;
    // K16 = 0x00 0x00 0x00 0x00 0x00 0x02
    keyring_expected[0].u8[5] = 0x02;

    des_generate_decryption_keyring(keyring, &initial_key);
    for (int i = 0; i < 16; ++i) {
        EXPECT_EQ(memcmp(keyring[i].u8, keyring_expected[i].u8, 6), 0);
    }
}

TEST(des, permutation) {
    des_block_t block, permuted, expected_initial, expected_final;
    block.u64 = 0;
    expected_initial.u64 = 0;
    expected_final.u64 = 0;
    block.u8[7] = 0x40;
    expected_initial.u8[0] = 0x80;
    expected_final.u8[6] = 0x02;

    des_block_permutation(&permuted, &block,
                          DES_INITIAL_PERMUTATION_CODE);
    EXPECT_EQ(permuted.u64, expected_initial.u64);
    des_block_permutation(&permuted, &block,
                          DES_FINAL_PERMUTATION_CODE);
    EXPECT_EQ(permuted.u64, expected_final.u64);
}

TEST(des, symmetry) {
    des_block_t plain_text, cipher_text, plain_text_decrypted;
    des_key_t key;
    char password[] = {'h', 'e', 'l', 'l', 'o', 'd', 'e', 's'};

    des_generate_key(password, &key);
    for (int i = 0; i < 8; ++i) {
        plain_text.u8[i] = (__uint8_t) (rand() % 256);
    }

    des_encrypt(&cipher_text, &plain_text, &key);
    des_decrypt(&plain_text_decrypted, &cipher_text, &key);
    EXPECT_EQ(plain_text.u64, plain_text_decrypted.u64);
}