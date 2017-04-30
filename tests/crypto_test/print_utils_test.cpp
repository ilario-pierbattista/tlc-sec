#include <gtest/gtest.h>

extern "C" {
#include "lib/print_utils.h"
};

TEST(print_utils, print_bin) {
    __uint8_t bytes[4] = {0x00, 0xFF, 0x55, 0xAA};
    std::string expected = "00000000 11111111 01010101 10101010 \n";

    testing::internal::CaptureStdout();
    print_bin(bytes, 4);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output == expected);
}

TEST(print_utils, print_hex) {
    __uint8_t bytes[4] = {0x00, 0xFF, 0x55, 0xAA};
    std::string expected = "00 FF 55 AA \n";

    testing::internal::CaptureStdout();
    print_hex(bytes, 4);
    std::string output = testing::internal::GetCapturedStdout();

    EXPECT_TRUE(output == expected);
}