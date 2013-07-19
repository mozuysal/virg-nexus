/**
 * @file tests_filter.cc
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Izmir Institute of Technology. All rights reserved.
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
static const short TEST_KERNEL_SI[KN] = { 3, 2, 1};
static const short TEST_KERNEL_SI_SUM = 9;
static const float TEST_KERNEL_S[KN] = { 3.0f/9.0f, 2.0f/9.0f, 1.0f/9.0f };

static const uchar BUFFER_GT_BZ[BN] = { 0, 0, 10, 20, 30, 40, 50, 0, 0 };
static const uchar BUFFER_GT_BR[BN] = { 10, 10, 10, 20, 30, 40, 50, 50, 50 };
static const uchar BUFFER_GT_BM[BN] = { 30, 20, 10, 20, 30, 40, 50, 40, 30 };

static const uchar BUFFER2_GT_BZ[BN2] = { 0, 0, 10, 30, 50, 0, 0 };
static const uchar BUFFER2_GT_BR[BN2] = { 10, 10, 10, 30, 50, 50, 50 };
static const uchar BUFFER2_GT_BM[BN2] = { 50, 30, 10, 30, 50, 30, 10 };

static const uchar CONV_SYM_SI_GT[BN] = { 11, 20, 30, 33, 28, 40, 50, 0, 0 };
static const uchar CONV_SYM_S_GT[BN]  = { 11, 20, 29, 33, 28, 40, 50, 0, 0 };


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

    uchar *buffer_;
};

TEST_F(NXFilterTest, BufferAlloc) {
    memset(buffer_, 0, BN*sizeof(uchar));
}

TEST_F(NXFilterTest, BufferCopy1BorderZero) {
    nx_filter_copy_to_buffer1_uc(N, buffer_, TEST_DATA, N_BORDER, NX_BORDER_ZERO);
    for (int i = 0; i < BN; ++i)
        EXPECT_EQ(BUFFER_GT_BZ[i], buffer_[i]);
    memset(buffer_, 0, BN*sizeof(uchar));
}

TEST_F(NXFilterTest, BufferCopy1BorderRepeat) {
    nx_filter_copy_to_buffer1_uc(N, buffer_, TEST_DATA, N_BORDER, NX_BORDER_REPEAT);
    for (int i = 0; i < BN; ++i)
        EXPECT_EQ(BUFFER_GT_BR[i], buffer_[i]);
    memset(buffer_, 0, BN*sizeof(uchar));
}

TEST_F(NXFilterTest, BufferCopy1BorderMirror) {
    nx_filter_copy_to_buffer1_uc(N, buffer_, TEST_DATA, N_BORDER, NX_BORDER_MIRROR);
    for (int i = 0; i < BN; ++i)
        EXPECT_EQ(BUFFER_GT_BM[i], buffer_[i]);
    memset(buffer_, 0, BN*sizeof(uchar));
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

TEST_F(NXFilterTest, BufferConvolveSumSI) {
    nx_filter_copy_to_buffer1_uc(N, buffer_, TEST_DATA, N_BORDER, NX_BORDER_ZERO);
    nx_convolve_sym_si_uc(N, buffer_, KN, TEST_KERNEL_SI, TEST_KERNEL_SI_SUM);
    for (int i = 0; i < BN; ++i)
        EXPECT_EQ(CONV_SYM_SI_GT[i], buffer_[i]);
    memset(buffer_, 0, BN*sizeof(uchar));
}

TEST_F(NXFilterTest, BufferConvolveSumS) {
    nx_filter_copy_to_buffer1_uc(N, buffer_, TEST_DATA, N_BORDER, NX_BORDER_ZERO);
    nx_convolve_sym_s_uc(N, buffer_, KN, TEST_KERNEL_S);
    for (int i = 0; i < BN; ++i)
        EXPECT_EQ(CONV_SYM_S_GT[i], buffer_[i]);
    memset(buffer_, 0, BN*sizeof(uchar));
}

} // namespace
