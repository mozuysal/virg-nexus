/**
 * @file tests_mem_block.cc
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Mustafa Ozuysal. All rights reserved.
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

#include "gtest/gtest.h"

#include "virg/nexus/nx_mem_block.h"

using namespace std;

namespace {

#define N_TEST_DATA 12
static const char TEST_GROUND_TRUTH[N_TEST_DATA] = { 123, 23, 12, 125, -122, -17, 16, 87, -45, 93, 20, 23 };
static char g_test_data[N_TEST_DATA];

class NXMemBlockTest : public ::testing::Test {
protected:
    NXMemBlockTest()
    {
        m0_ = m1_ = m2_ = NULL;
    }

    virtual void SetUp()
    {
        memcpy(g_test_data, TEST_GROUND_TRUTH, N_TEST_DATA);

        m0_ = nx_mem_block_new(N_TEST_DATA);
        m1_ = nx_mem_block_alloc();
        m2_ = nx_mem_block_alloc();

        for (int i = 0; i < N_TEST_DATA; ++i) {
            m0_->ptr[i] = g_test_data[i];
        }
        nx_mem_block_wrap(m1_, &g_test_data[0], N_TEST_DATA, N_TEST_DATA, NX_FALSE);
    }

    virtual void TearDown()
    {
        nx_mem_block_free(m0_);
        nx_mem_block_free(m1_);
        nx_mem_block_free(m2_);
    }

    struct NXMemBlock *m0_;
    struct NXMemBlock *m1_;
    struct NXMemBlock *m2_;
};

TEST_F(NXMemBlockTest, Create) {
    EXPECT_EQ((size_t)N_TEST_DATA, m0_->size);
    EXPECT_TRUE(NULL != m0_->ptr);
    EXPECT_TRUE(m0_->own_memory);
    for (int i = 0; i < N_TEST_DATA; ++i ) {
        EXPECT_EQ(TEST_GROUND_TRUTH[i], m0_->ptr[i]);
    }
    nx_mem_block_set_zero(m0_);
}

TEST_F(NXMemBlockTest, Release) {
    nx_mem_block_release(m0_);
    EXPECT_EQ(0U, m0_->size);
    EXPECT_EQ(0U, m0_->capacity);
    EXPECT_TRUE(NULL == m0_->ptr);
}

TEST_F(NXMemBlockTest, ResizeSmaller) {
    nx_mem_block_resize(m0_, N_TEST_DATA-1);
    EXPECT_EQ((size_t)(N_TEST_DATA-1), m0_->size);
    EXPECT_TRUE(NULL != m0_->ptr);
    EXPECT_TRUE(m0_->own_memory);
    nx_mem_block_set_zero(m0_);
}

TEST_F(NXMemBlockTest, ResizeLarger) {
    nx_mem_block_resize(m0_, N_TEST_DATA+1);
    EXPECT_EQ((size_t)(N_TEST_DATA+1), m0_->size);
    EXPECT_TRUE(NULL != m0_->ptr);
    EXPECT_TRUE(m0_->own_memory);
    nx_mem_block_set_zero(m0_);
}

TEST_F(NXMemBlockTest, Wrap) {
    EXPECT_EQ((size_t)N_TEST_DATA, m1_->size);
    EXPECT_EQ((size_t)N_TEST_DATA, m1_->capacity);
    EXPECT_EQ(g_test_data, m1_->ptr);
    EXPECT_FALSE(m1_->own_memory);
    nx_mem_block_set_zero(m1_);
    for (int i = 0; i < N_TEST_DATA; ++i ) {
        EXPECT_EQ(0, g_test_data[i]);
    }
}

TEST_F(NXMemBlockTest, WrapCreate) {
    nx_mem_block_release(m1_);
    nx_mem_block_resize(m1_, N_TEST_DATA-1);
    EXPECT_EQ((size_t)(N_TEST_DATA-1), m1_->size);
    EXPECT_NE(g_test_data, m1_->ptr);
    EXPECT_TRUE(m1_->own_memory);
    nx_mem_block_set_zero(m1_);
}

TEST_F(NXMemBlockTest, WrapResizeSmaller) {
    nx_mem_block_resize(m1_, N_TEST_DATA-1);
    EXPECT_EQ((size_t)(N_TEST_DATA-1), m1_->size);
    EXPECT_EQ(g_test_data, m1_->ptr);
    EXPECT_FALSE(m1_->own_memory);
    nx_mem_block_set_zero(m1_);
    for (int i = 0; i < N_TEST_DATA-1; ++i ) {
        EXPECT_EQ(0, g_test_data[i]);
    }
    EXPECT_EQ(g_test_data[N_TEST_DATA-1], TEST_GROUND_TRUTH[N_TEST_DATA-1]);
}

TEST_F(NXMemBlockTest, WrapResizeLarger) {
    nx_mem_block_resize(m1_, N_TEST_DATA+1);
    EXPECT_EQ((size_t)(N_TEST_DATA+1), m1_->size);
    EXPECT_NE(g_test_data, m1_->ptr);
    EXPECT_TRUE(m1_->own_memory);
    for (int i = 0; i < N_TEST_DATA; ++i ) {
        EXPECT_EQ(g_test_data[i], m1_->ptr[i]);
    }
    nx_mem_block_set_zero(m1_);
}

TEST_F(NXMemBlockTest, SwapWith) {
    nx_mem_block_swap(m0_, m1_);
    EXPECT_EQ((size_t)N_TEST_DATA, m0_->size);
    EXPECT_EQ((size_t)N_TEST_DATA, m0_->capacity);
    EXPECT_TRUE(g_test_data == m0_->ptr);
    EXPECT_FALSE(m0_->own_memory);
    for (int i = 0; i < N_TEST_DATA; ++i ) {
        EXPECT_EQ(TEST_GROUND_TRUTH[i], m0_->ptr[i]);
    }
    EXPECT_EQ((size_t)N_TEST_DATA, m1_->size);
    EXPECT_TRUE(NULL != m1_->ptr);
    EXPECT_TRUE(m1_->own_memory);
    for (int i = 0; i < N_TEST_DATA; ++i ) {
        EXPECT_EQ(TEST_GROUND_TRUTH[i], m1_->ptr[i]);
    }
    nx_mem_block_set_zero(m0_);
    nx_mem_block_set_zero(m1_);
}

TEST_F(NXMemBlockTest, SwapWith0) {
    nx_mem_block_swap(m0_, m2_);
    EXPECT_EQ(0U, m0_->size);
    EXPECT_EQ(0U, m0_->capacity);
    EXPECT_TRUE(NULL == m0_->ptr);
    EXPECT_EQ((size_t)N_TEST_DATA, m2_->size);
    EXPECT_TRUE(NULL != m2_->ptr);
    EXPECT_TRUE(m2_->own_memory);
    for (int i = 0; i < N_TEST_DATA; ++i ) {
        EXPECT_EQ(TEST_GROUND_TRUTH[i], m2_->ptr[i]);
    }
}

TEST_F(NXMemBlockTest, CopyOf) {
    nx_mem_block_copy(m2_, m0_);
    EXPECT_EQ((size_t)N_TEST_DATA, m2_->size);
    EXPECT_TRUE(NULL != m2_->ptr);
    EXPECT_TRUE(m0_->ptr != m2_->ptr);
    EXPECT_TRUE(m2_->own_memory);
    for (int i = 0; i < N_TEST_DATA; ++i ) {
        EXPECT_EQ(TEST_GROUND_TRUTH[i], m2_->ptr[i]);
    }
    nx_mem_block_set_zero(m0_);
    nx_mem_block_set_zero(m2_);
}

TEST_F(NXMemBlockTest, CopyOfWrapped) {
    nx_mem_block_copy(m2_, m1_);
    EXPECT_EQ((size_t)N_TEST_DATA, m2_->size);
    EXPECT_TRUE(NULL != m2_->ptr);
    EXPECT_TRUE(g_test_data != m2_->ptr);
    EXPECT_TRUE(m2_->own_memory);
    for (int i = 0; i < N_TEST_DATA; ++i ) {
        EXPECT_EQ(TEST_GROUND_TRUTH[i], m2_->ptr[i]);
    }
    nx_mem_block_set_zero(m2_);
    for (int i = 0; i < N_TEST_DATA; ++i ) {
        EXPECT_EQ(TEST_GROUND_TRUTH[i], g_test_data[i]);
    }
}

} // namespace
