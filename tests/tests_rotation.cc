/**
 * @file tests_rotation.cc
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
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
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
#include "virg/nexus/nx_mat234.h"
#include "virg/nexus/nx_math.h"
#include "virg/nexus/nx_rotation_3d.h"

static const double TOL = 1e-15;

using namespace std;

namespace {

class NXRotation3DTest : public ::testing::Test {
protected:
        NXRotation3DTest() {
        }

        virtual void SetUp() {
                X[0] = 1.0;
                X[1] = 2.0;
                X[2] = 3.0;
        }

        virtual void TearDown() {
        }

        void assert_identity(double *A) {
                EXPECT_GT(TOL, fabs(1.0 - A[0]));
                EXPECT_GT(TOL, fabs(0.0 - A[1]));
                EXPECT_GT(TOL, fabs(0.0 - A[2]));
                EXPECT_GT(TOL, fabs(0.0 - A[3]));
                EXPECT_GT(TOL, fabs(1.0 - A[4]));
                EXPECT_GT(TOL, fabs(0.0 - A[5]));
                EXPECT_GT(TOL, fabs(0.0 - A[6]));
                EXPECT_GT(TOL, fabs(0.0 - A[7]));
                EXPECT_GT(TOL, fabs(1.0 - A[8]));
        }

        void so3_xyz_test(double alpha, double beta, double gamma) {
                nx_rotation_3d_RxRyRz(&R[0], alpha, beta, gamma);
                memcpy(&Rt[0], &R[0], 9*sizeof(Rt[0]));
                nx_rotation_3d_invert(Rt);

                EXPECT_GT(TOL, fabs(+1 - nx_dmat3_det(&R[0])));
                EXPECT_GT(TOL, fabs(+1 - nx_dmat3_det(&Rt[0])));

                double I[9];
                nx_dmat3_mul(&I[0], &R[0], &Rt[0]);
                assert_identity(&I[0]);
                nx_dmat3_mul(&I[0], &Rt[0], &R[0]);
                assert_identity(&I[0]);
        }

        void so3_zyx_test(double alpha, double beta, double gamma) {
                nx_rotation_3d_RzRyRx(&R[0], alpha, beta, gamma);
                memcpy(&Rt[0], &R[0], 9*sizeof(Rt[0]));
                nx_rotation_3d_invert(Rt);

                EXPECT_GT(TOL, fabs(+1 - nx_dmat3_det(&R[0])));
                EXPECT_GT(TOL, fabs(+1 - nx_dmat3_det(&Rt[0])));

                double I[9];
                nx_dmat3_mul(&I[0], &R[0], &Rt[0]);
                assert_identity(&I[0]);
                nx_dmat3_mul(&I[0], &Rt[0], &R[0]);
                assert_identity(&I[0]);
        }

        void rotate_X() {
                XP[0] = R[0]*X[0] + R[3]*X[1] + R[6]*X[2];
                XP[1] = R[1]*X[0] + R[4]*X[1] + R[7]*X[2];
                XP[2] = R[2]*X[0] + R[5]*X[1] + R[8]*X[2];
        }

        double R[9];
        double Rt[9];
        double X[3];
        double XP[3];
};

TEST_F(NXRotation3DTest, RxRyRz_test) {
        nx_rotation_3d_RxRyRz(&R[0], 0.0, 0.0, 0.0);
        assert_identity(&R[0]);

        for (int alpha = -1; alpha < 2; alpha++) {
                for (int beta = -1; alpha < 2; alpha++) {
                        for (int gamma = -1; alpha < 2; alpha++) {
                                so3_xyz_test(alpha*NX_PI/6,
                                             beta*NX_PI/3,
                                             gamma*NX_PI/4);
                        }
                }
        }
}

TEST_F(NXRotation3DTest, RzRyRx_test) {
        nx_rotation_3d_RzRyRx(&R[0], 0.0, 0.0, 0.0);
        assert_identity(&R[0]);

        for (int alpha = -1; alpha < 2; alpha++) {
                for (int beta = -1; alpha < 2; alpha++) {
                        for (int gamma = -1; alpha < 2; alpha++) {
                                so3_zyx_test(alpha*NX_PI/6,
                                             beta*NX_PI/3,
                                             gamma*NX_PI/4);
                        }
                }
        }
}


} // namespace
