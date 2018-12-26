/**
 * @file tests_filter.cc
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

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_filter.h"

using namespace std;

namespace {

#define N 5
#define KN 3
#define N_BORDER (KN-1)
#define BN ((N) + 2*N_BORDER)
#define BN2 (((N+1)/2) + 2*N_BORDER)

static const uchar TEST_DATA[N] = { 10, 20, 30, 40, 50 };
static const float TEST_KERNEL_S[KN] = { 3.0f/9.0f, 2.0f/9.0f, 1.0f/9.0f };

static const float BUFFER_GT_BZ[BN] = { 0.0f, 0.0f, 10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 0.0f, 0.0f };
static const float BUFFER_GT_BR[BN] = { 10.0f, 10.0f, 10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 50.0f, 50.0f };
static const float BUFFER_GT_BM[BN] = { 30.0f, 20.0f, 10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 40.0f, 30.0f };

static const float BUFFER2_GT_BZ[BN2] = { 0.0f, 0.0f, 10.0f, 30.0f, 50.0f, 0.0f, 0.0f };
static const float BUFFER2_GT_BR[BN2] = { 10.0f, 10.0f, 10.0f, 30.0f, 50.0f, 50.0f, 50.0f };
static const float BUFFER2_GT_BM[BN2] = { 50.0f, 30.0f, 10.0f, 30.0f, 50.0f, 30.0f, 10.0f };

static const float CONV_SYM_GT[BN]  = { 100.0/9.0f, 20.0f, 30.0f, 300.0f/9.0f, 260.0f/9.0f, 40.0f, 50.0f, 0.0f, 0.0f };


class NXFilterTest : public ::testing::Test {
protected:
    NXFilterTest()
    {
        buffer_ = NULL;
    }

    virtual void SetUp()
    {
        buffer_ = nx_filter_buffer_alloc(N, N_BORDER);
    }

    virtual void TearDown()
    {
        nx_free(buffer_);
    }

    float *buffer_;
};

TEST_F(NXFilterTest, BufferAlloc) {
    memset(buffer_, 0, BN*sizeof(float));
}

TEST_F(NXFilterTest, BufferCopy1BorderZero) {
    nx_filter_copy_to_buffer1_uc(N, buffer_, TEST_DATA, N_BORDER, NX_BORDER_ZERO);
    for (int i = 0; i < BN; ++i)
        EXPECT_EQ(BUFFER_GT_BZ[i], buffer_[i]);
    memset(buffer_, 0, BN*sizeof(float));
}

TEST_F(NXFilterTest, BufferCopy1BorderRepeat) {
    nx_filter_copy_to_buffer1_uc(N, buffer_, TEST_DATA, N_BORDER, NX_BORDER_REPEAT);
    for (int i = 0; i < BN; ++i)
        EXPECT_EQ(BUFFER_GT_BR[i], buffer_[i]);
    memset(buffer_, 0, BN*sizeof(float));
}

TEST_F(NXFilterTest, BufferCopy1BorderMirror) {
    nx_filter_copy_to_buffer1_uc(N, buffer_, TEST_DATA, N_BORDER, NX_BORDER_MIRROR);
    for (int i = 0; i < BN; ++i)
        EXPECT_EQ(BUFFER_GT_BM[i], buffer_[i]);
    memset(buffer_, 0, BN*sizeof(float));
}

TEST_F(NXFilterTest, BufferCopyBorderZero) {
    nx_filter_copy_to_buffer_uc((N+1)/2, buffer_, TEST_DATA, 2, N_BORDER, NX_BORDER_ZERO);
    for (int i = 0; i < BN2; ++i)
        EXPECT_EQ(BUFFER2_GT_BZ[i], buffer_[i]);
    memset(buffer_, 0, BN2*sizeof(uchar));
}

TEST_F(NXFilterTest, BufferCopyBorderRepeat) {
    nx_filter_copy_to_buffer_uc((N+1)/2, buffer_, TEST_DATA, 2, N_BORDER, NX_BORDER_REPEAT);
    for (int i = 0; i < BN2; ++i)
        EXPECT_EQ(BUFFER2_GT_BR[i], buffer_[i]);
    memset(buffer_, 0, BN2*sizeof(uchar));
}

TEST_F(NXFilterTest, BufferCopyBorderMirror) {
    nx_filter_copy_to_buffer_uc((N+1)/2, buffer_, TEST_DATA, 2, N_BORDER, NX_BORDER_MIRROR);
    for (int i = 0; i < BN2; ++i)
        EXPECT_EQ(BUFFER2_GT_BM[i], buffer_[i]);
    memset(buffer_, 0, BN2*sizeof(uchar));
}

TEST_F(NXFilterTest, BufferConvolveSym) {
    nx_filter_copy_to_buffer1_uc(N, buffer_, TEST_DATA, N_BORDER, NX_BORDER_ZERO);
    nx_convolve_sym(N, buffer_, KN, TEST_KERNEL_S);
    for (int i = 0; i < BN; ++i)
        EXPECT_FLOAT_EQ(CONV_SYM_GT[i], buffer_[i]);
    memset(buffer_, 0, BN*sizeof(float));
}

} // namespace
