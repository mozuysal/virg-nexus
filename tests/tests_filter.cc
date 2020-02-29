/**
 * @file tests_filter.cc
 *
 * Copyright (C) 2019 Mustafa Ozuysal. All rights reserved.
 *
 * This file is part of the VIRG-Nexus Library
 *
 * VIRG-Nexus Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VIRG-Nexus Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
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
#define KR 2
#define N_BORDER (KN-1)
#define BN ((N) + 2*N_BORDER)
#define BN2 (((N+1)/2) + 2*N_BORDER)
#define BRN ((N)+2*KR)

static const uchar TEST_DATA[N] = { 10, 20, 30, 40, 50 };
static const float TEST_KERNEL_S[KN] = { 3.0f/9.0f, 2.0f/9.0f, 1.0f/9.0f };

static const float BUFFER_GT_BZ[BN] = { 0.0f, 0.0f, 10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 0.0f, 0.0f };
static const float BUFFER_GT_BR[BN] = { 10.0f, 10.0f, 10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 50.0f, 50.0f };
static const float BUFFER_GT_BM[BN] = { 30.0f, 20.0f, 10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 40.0f, 30.0f };

static const float BUFFER2_GT_BZ[BN2] = { 0.0f, 0.0f, 10.0f, 30.0f, 50.0f, 0.0f, 0.0f };
static const float BUFFER2_GT_BR[BN2] = { 10.0f, 10.0f, 10.0f, 30.0f, 50.0f, 50.0f, 50.0f };
static const float BUFFER2_GT_BM[BN2] = { 50.0f, 30.0f, 10.0f, 30.0f, 50.0f, 30.0f, 10.0f };

static const float CONV_SYM_GT[BN]  = { 100.0/9.0f, 20.0f, 30.0f, 300.0f/9.0f, 260.0f/9.0f, 40.0f, 50.0f, 0.0f, 0.0f };
static const float CONV_BOX_GT[BRN]  = { 60.0f/5.0f, 100.0f/5.0f, 150.0f/5.0f, 140.0f/5.0f, 120.0f/5.0f, 40.0f, 50.0f, 0.0f, 0.0f };

class NXFilterTest : public ::testing::Test {
protected:
        NXFilterTest() {
                buffer_ = NULL;
                box_buffer_ = NULL;
        }

        virtual void SetUp() {
                buffer_ = nx_filter_buffer_alloc(N, N_BORDER);
                box_buffer_ = nx_filter_buffer_alloc(N, KR);
        }

        virtual void TearDown() {
                nx_free(buffer_);
                nx_free(box_buffer_);
        }

        float *buffer_;
        float *box_buffer_;
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

TEST_F(NXFilterTest, BufferConvolveBox) {
    nx_filter_copy_to_buffer1_uc(N, box_buffer_, TEST_DATA, KR, NX_BORDER_ZERO);
    nx_convolve_box(N, box_buffer_, KR);
    for (int i = 0; i < BRN; ++i)
        EXPECT_FLOAT_EQ(CONV_BOX_GT[i], box_buffer_[i]);
    memset(box_buffer_, 0, BRN*sizeof(float));
}

} // namespace
