/**
 * @file tests_svd.cc
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

#include "gtest/gtest.h"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_svd.h"

#define TEST_M 3
#define TEST_N 4
#define TEST_MN ((TEST_M)*(TEST_N))

using namespace std;

namespace {

static double As[TEST_MN] = {
        1, 1, 5,
        2, 2, 6,
        3, 3, 7,
        4, 4 ,8
};

static double Us[TEST_M*TEST_M] = {
        -3.53912572757243e-01, -3.53912572757242e-01, -8.65731934081618e-01,
        -6.12164921278857e-01, -6.12164921278858e-01, 5.00507960287647e-01,
        -7.07106781186548e-01, 7.07106781186547e-01,  2.77555756156289e-16
};

static double Ss[TEST_M] = {
        1.52063216993135e+01, 1.66366474296603e+00, 4.51923530615663e-16
};

static double Vts[TEST_N*TEST_N] = {
        -3.31209934625410e-01,  7.68309819802813e-01, -5.32885859913642e-01, -1.26620141778859e-01,
        -4.34690389051621e-01,  3.33233050080797e-01,  6.16137398438792e-01,  5.66016524710257e-01,
        -5.38170843477831e-01, -1.01843719641221e-01,  3.66382782863341e-01, -7.52172624083936e-01,
        -6.41651297904042e-01, -5.36920489363238e-01, -4.49634321388491e-01,  3.12776241152538e-01
};

class NXSVDTest : public ::testing::Test {
protected:
        NXSVDTest() {
        }

        virtual void SetUp() {
                memcpy(A, As, TEST_MN*sizeof(double));
        }

        virtual void TearDown() {
        }

        double A[TEST_MN];
        double U[TEST_M*TEST_M];
        double S[TEST_M];
        double Vt[TEST_N*TEST_N];
};

TEST_F(NXSVDTest, SVD_USVt) {
        nx_dsvd_usvt(U, TEST_M, S, Vt, TEST_N, TEST_M, TEST_N, &A[0], TEST_M);

        double abs_diff_sum = 0.0;
        for (int i = 0; i < TEST_M; ++i)
                for (int j = 0; j < TEST_M; ++j)
                        abs_diff_sum += fabs(U[j*TEST_M+i] - Us[j*TEST_M+i]);
        /* printf("U diff = %.12le\n", abs_diff_sum); */
        EXPECT_GT(1e-12, abs_diff_sum);

        abs_diff_sum = 0.0;
        for (int i = 0; i < TEST_M; ++i)
                abs_diff_sum += fabs(S[i] - Ss[i]);
        /* printf("S diff = %.12le\n", abs_diff_sum); */
        EXPECT_GT(1e-12, abs_diff_sum);

        abs_diff_sum = 0.0;
        for (int i = 0; i < TEST_N; ++i)
                for (int j = 0; j < TEST_N; ++j)
                        abs_diff_sum += fabs(Vt[j*TEST_N+i] - Vts[j*TEST_N+i]);
        /* printf("Vt diff = %.12le\n", abs_diff_sum); */
        EXPECT_GT(1e-12, abs_diff_sum);
}

TEST_F(NXSVDTest, SVD_SVt) {
        nx_dsvd_svt(S, Vt, TEST_N, TEST_M, TEST_N, &A[0], TEST_M);

        double abs_diff_sum = 0.0;
        for (int i = 0; i < TEST_M; ++i)
                abs_diff_sum += fabs(S[i] - Ss[i]);
        /* printf("S diff = %.12le\n", abs_diff_sum); */
        EXPECT_GT(1e-12, abs_diff_sum);

        abs_diff_sum = 0.0;
        for (int i = 0; i < TEST_N; ++i)
                for (int j = 0; j < TEST_N; ++j)
                        abs_diff_sum += fabs(Vt[j*TEST_N+i] - Vts[j*TEST_N+i]);
        /* printf("Vt diff = %.12le\n", abs_diff_sum); */
        EXPECT_GT(1e-12, abs_diff_sum);
}

TEST_F(NXSVDTest, SVD_US) {
        nx_dsvd_us(U, TEST_M, S, TEST_M, TEST_N, &A[0], TEST_M);

        double abs_diff_sum = 0.0;
        for (int i = 0; i < TEST_M; ++i)
                for (int j = 0; j < TEST_M; ++j)
                        abs_diff_sum += fabs(U[j*TEST_M+i] - Us[j*TEST_M+i]);
        /* printf("U diff = %.12le\n", abs_diff_sum); */
        EXPECT_GT(1e-12, abs_diff_sum);

        abs_diff_sum = 0.0;
        for (int i = 0; i < TEST_M; ++i)
                abs_diff_sum += fabs(S[i] - Ss[i]);
        /* printf("S diff = %.12le\n", abs_diff_sum); */
        EXPECT_GT(1e-12, abs_diff_sum);
}

TEST_F(NXSVDTest, SVD_ONLY_S) {
        nx_dsvd_only_s(S, TEST_M, TEST_N, &A[0], TEST_M);

        double abs_diff_sum = 0.0;
        for (int i = 0; i < TEST_M; ++i)
                abs_diff_sum += fabs(S[i] - Ss[i]);
        /* printf("S diff = %.12le\n", abs_diff_sum); */
        EXPECT_GT(1e-12, abs_diff_sum);
}

} // namespace
