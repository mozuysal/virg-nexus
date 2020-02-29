/**
 * @file tests_svd.cc
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
#include "virg/nexus/nx_svd.h"

#define TEST_M 3
#define TEST_N 4
#define TEST_MN ((TEST_M)*(TEST_N))
#define MAX_ABS_ERROR_TOL 1.0e-12

using namespace std;

namespace {

static double As[TEST_MN] = {
        1, 1, 5,
        2, 2, 6,
        3, 3, 7,
        4, 4 ,8
};

static double Ss[TEST_M] = {
        1.52063216993135e+01, 1.66366474296603e+00, 4.51923530615663e-16
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

        void compute_XXt(double *XXt, int n, const double *X) {
                for (int i = 0; i < n; ++i) {
                        for (int j = 0; j < n; ++j) {
                                XXt[j+i*n] = 0.0;
                                for (int k = 0; k < n; ++k) {
                                        XXt[j+i*n] += X[j+n*k]*X[i+n*k];
                                }
                        }
                }
        }

        double max_abs_error(int n, const double *XXt) {
                double e = 0.0;
                for (int i = 0; i < n; ++i) {
                        for (int j = 0; j < n; ++j) {
                                double gt = (i == j) ? 1.0 : 0.0;
                                double abs_e = fabs(gt - XXt[i*n+j]);
                                if (abs_e > e)
                                        e = abs_e;
                        }
                }

                return e;
        }

        double max_abs_error_U() {
                double UUt[TEST_M*TEST_M];
                compute_XXt(UUt, TEST_M, U);
                return max_abs_error(TEST_M, UUt);
        }

        double max_abs_error_Vt() {
                double VtV[TEST_N*TEST_N];
                compute_XXt(VtV, TEST_N, Vt);
                return max_abs_error(TEST_N, VtV);
        }

        double max_abs_error_S() {
                double e = 0.0;
                for (int i = 0; i < TEST_M; ++i) {
                        double abs_e = fabs(S[i] - Ss[i]);
                        if (abs_e > e)
                                e = abs_e;
                }

                return e;
        }

        double max_abs_error_A() {
                double Asvd[TEST_M*TEST_N];
                for (int i = 0; i < TEST_N; ++i) {
                        for (int j = 0; j < TEST_M; ++j) {
                                Asvd[j+i*TEST_M] = 0.0;
                                for (int k = 0; k < TEST_M; ++k) {
                                        Asvd[j+i*TEST_M] += U[j+TEST_M*k]*S[k]*Vt[k+TEST_N*i];
                                }
                        }
                }

                double e = 0.0;
                for (int i = 0; i < TEST_N; ++i) {
                        for (int j = 0; j < TEST_M; ++j) {
                                double abs_e = fabs(Asvd[i*TEST_M+j] - As[i*TEST_M+j]);
                                if (abs_e > e)
                                        e = abs_e;
                        }
                }

                return e;
        }

        double A[TEST_MN];
        double U[TEST_M*TEST_M];
        double S[TEST_M];
        double Vt[TEST_N*TEST_N];
};

TEST_F(NXSVDTest, SVD_USVt) {
        nx_dsvd_usvt(U, TEST_M, S, Vt, TEST_N, TEST_M, TEST_N, &A[0], TEST_M);

        EXPECT_GT(MAX_ABS_ERROR_TOL, max_abs_error_U());
        EXPECT_GT(MAX_ABS_ERROR_TOL, max_abs_error_S());
        EXPECT_GT(MAX_ABS_ERROR_TOL, max_abs_error_Vt());
        EXPECT_GT(MAX_ABS_ERROR_TOL, max_abs_error_A());
}

TEST_F(NXSVDTest, SVD_SVt) {
        nx_dsvd_svt(S, Vt, TEST_N, TEST_M, TEST_N, &A[0], TEST_M);

        EXPECT_GT(MAX_ABS_ERROR_TOL, max_abs_error_S());
        EXPECT_GT(MAX_ABS_ERROR_TOL, max_abs_error_Vt());
}

TEST_F(NXSVDTest, SVD_US) {
        nx_dsvd_us(U, TEST_M, S, TEST_M, TEST_N, &A[0], TEST_M);

        EXPECT_GT(MAX_ABS_ERROR_TOL, max_abs_error_U());
        EXPECT_GT(MAX_ABS_ERROR_TOL, max_abs_error_S());
}

TEST_F(NXSVDTest, SVD_ONLY_S) {
        nx_dsvd_only_s(S, TEST_M, TEST_N, &A[0], TEST_M);

        EXPECT_GT(MAX_ABS_ERROR_TOL, max_abs_error_S());
}

} // namespace
