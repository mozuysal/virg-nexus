/**
 * @file tests_bit_ops.cc
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2015 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <ctime>
#include <cinttypes>

#include "gtest/gtest.h"

#include "virg/nexus/nx_bit_ops.h"

using namespace std;

namespace {

class NXBitOpsTest : public ::testing::Test {
protected:
        NXBitOpsTest() {
        }

        virtual void SetUp() {
        }

        virtual void TearDown() {
        }
};

TEST_F(NXBitOpsTest, zero32) {
        uint32_t x = 0;
        int bc = nx_bit_count32(x);
        EXPECT_EQ(bc, 0);
}

TEST_F(NXBitOpsTest, zero64) {
        uint64_t x = 0;
        int bc = nx_bit_count64(x);
        EXPECT_EQ(bc, 0);
}

TEST_F(NXBitOpsTest, ones32) {
        for (int i = 0; i < 32; ++i) {
                uint32_t x = (uint32_t)1 << i;
                int bc = nx_bit_count32(x);
                EXPECT_EQ(bc, 1);
        }
}

TEST_F(NXBitOpsTest, ones64) {
        for (int i = 0; i < 64; ++i) {
                uint64_t x = (uint64_t)1 << i;
                int bc = nx_bit_count64(x);
                EXPECT_EQ(bc, 1);
        }
}

TEST_F(NXBitOpsTest, patterns32) {
        uint32_t x = 0xFF00FF00;
        int bc = nx_bit_count32(x);
        EXPECT_EQ(bc, 16);

        x = 0x00FF00FF;
        bc = nx_bit_count32(x);
        EXPECT_EQ(bc, 16);

        x = 0xF0F0F0F0;
        bc = nx_bit_count32(x);
        EXPECT_EQ(bc, 16);

        x = 0x0F0F0F0F;
        bc = nx_bit_count32(x);
        EXPECT_EQ(bc, 16);
}

TEST_F(NXBitOpsTest, patterns64) {
        uint64_t x = 0xFF00FF00FF00FF00;
        int bc = nx_bit_count64(x);
        EXPECT_EQ(bc, 32);

        x = 0x00FF00FF00FF00FF;
        bc = nx_bit_count64(x);
        EXPECT_EQ(bc, 32);

        x = 0xF0F0F0F0F0F0F0F0;
        bc = nx_bit_count64(x);
        EXPECT_EQ(bc, 32);

        x = 0x0F0F0F0F0F0F0F0F;
        bc = nx_bit_count64(x);
        EXPECT_EQ(bc, 32);
}


} // namespace
