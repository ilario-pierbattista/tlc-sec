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
    // x000 0000
    // 0000 0000
    EXPECT_EQ(add_parity_bit((__uint8_t) 0x00), 0x00);
    // x000 0011
    // 0000 0011
    EXPECT_EQ(add_parity_bit((__uint8_t) 0x03), 0x03);
    // x110 1100
    // 0110 1100
    EXPECT_EQ(add_parity_bit((__uint8_t) 0x6C), 0x6C);
    // x111 1111
    // 1111 1111
    EXPECT_EQ(add_parity_bit((__uint8_t) 0x7F), 0xFF);
    // x000 0001
    // 1000 0001
    EXPECT_EQ(add_parity_bit((__uint8_t) 0x01), 0x81);
    // x011 1110
    // 1011 1110
    EXPECT_EQ(add_parity_bit((__uint8_t) 0x3E), 0xBE);
}

TEST(byte_utils, remove_parity) {
    // 0000 0000
    // x000 0000
    EXPECT_EQ(remove_parity_bit((__uint8_t) 0x00), 0x00);
    // 0000 0011
    // x000 0011
    EXPECT_EQ(remove_parity_bit((__uint8_t) 0x03), 0x03);
    // 0110 1100
    // x110 1100
    EXPECT_EQ(remove_parity_bit((__uint8_t) 0x6C), 0x6C);
    // 1111 1111
    // x111 1111
    EXPECT_EQ(remove_parity_bit((__uint8_t) 0xFF), 0x7F);
    // 1000 0001
    // x000 0001
    EXPECT_EQ(remove_parity_bit((__uint8_t) 0x81), 0x01);
    // 1011 1110
    // x011 1110
    EXPECT_EQ(remove_parity_bit((__uint8_t) 0xBE), 0x3E);
}