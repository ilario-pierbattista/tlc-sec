//
// Created by ilario on 28/04/17.
//

#include "gtest/gtest.h"

extern "C" {
#include "lib/byte_utils.h"
}

TEST(byte_utils, check_parity) {
    // 00000000
    EXPECT_TRUE(byte_parity_check((__uint8_t) 0x00));
    // 00000011
    EXPECT_TRUE(byte_parity_check((__uint8_t) 0x03));
    // 11111100
    EXPECT_TRUE(byte_parity_check((__uint8_t) 0xFC));
    // 11111111
    EXPECT_TRUE(byte_parity_check((__uint8_t) 0xFF));
    // 00000001
    EXPECT_FALSE(byte_parity_check((__uint8_t) 0x01));
    // 11111110
    EXPECT_FALSE(byte_parity_check((__uint8_t) 0xFE));
}

TEST(byte_utils, add_parity) {
    // 0000 000x
    // 0000 0000
    EXPECT_EQ(byte_add_parity_bit((__uint8_t) 0x00), 0x00);
    // x000 0011
    // 0000 0110
    EXPECT_EQ(byte_add_parity_bit((__uint8_t) 0x03), 0x06);
    // x110 1100
    // 1101 1000
    EXPECT_EQ(byte_add_parity_bit((__uint8_t) 0x6C), 0xD8);
    // x111 1111
    // 1111 1111
    EXPECT_EQ(byte_add_parity_bit((__uint8_t) 0x7F), 0xFF);
    // x000 0001
    // 0000 0011
    EXPECT_EQ(byte_add_parity_bit((__uint8_t) 0x01), 0x03);
    // x011 1110
    // 0111 1101
    EXPECT_EQ(byte_add_parity_bit((__uint8_t) 0x3E), 0x7D);
}

TEST(byte_utils, remove_parity) {
    // 0000 0000
    // x000 0000
    EXPECT_EQ(byte_remove_parity_bit((__uint8_t) 0x00), 0x00);
    // 0000 0011
    // x000 0001
    EXPECT_EQ(byte_remove_parity_bit((__uint8_t) 0x03), 0x01);
    // 0110 1100
    // x011 0110
    EXPECT_EQ(byte_remove_parity_bit((__uint8_t) 0x6C), 0x36);
    // 1111 1111
    // x111 1111
    EXPECT_EQ(byte_remove_parity_bit((__uint8_t) 0xFF), 0x7F);
    // 1000 0001
    // x100 0000
    EXPECT_EQ(byte_remove_parity_bit((__uint8_t) 0x81), 0x40);
    // 1011 1110
    // x101 1111
    EXPECT_EQ(byte_remove_parity_bit((__uint8_t) 0xBE), 0x5F);
}

TEST(byte_utils, get_bit) {
    EXPECT_EQ(byte_get_bit(0x01, 8), 0x01);
    EXPECT_EQ(byte_get_bit(0x02, 7), 0x01);
    EXPECT_EQ(byte_get_bit(0x04, 6), 0x01);
    EXPECT_EQ(byte_get_bit(0x08, 5), 0x01);
    EXPECT_EQ(byte_get_bit(0x10, 4), 0x01);
    EXPECT_EQ(byte_get_bit(0x20, 3), 0x01);
    EXPECT_EQ(byte_get_bit(0x40, 2), 0X01);
    EXPECT_EQ(byte_get_bit(0x80, 1), 0X01);
    EXPECT_EQ(byte_get_bit(0xFF, 8), 0X01);
    EXPECT_EQ(byte_get_bit(0xFF, 7), 0X01);
    EXPECT_EQ(byte_get_bit(0xFF, 6), 0X01);
    EXPECT_EQ(byte_get_bit(0xFF, 5), 0X01);
    EXPECT_EQ(byte_get_bit(0xFF, 4), 0X01);
    EXPECT_EQ(byte_get_bit(0xFF, 3), 0X01);
    EXPECT_EQ(byte_get_bit(0xFF, 2), 0X01);
    EXPECT_EQ(byte_get_bit(0xFF, 1), 0X01);
}