/**
 * @file tests_quaternion.cc
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
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_vec234.h"
#include "virg/nexus/nx_mat234.h"
#include "virg/nexus/nx_math.h"
#include "virg/nexus/nx_uniform_sampler.h"
#include "virg/nexus/nx_rotation_3d.h"
#include "virg/nexus/nx_quaternion.h"

#define ABS_MAX(x,y) ((fabs(x) > fabs(y)) ? fabs(x) : fabs(y))

extern bool IS_VALGRIND_RUN;
static const double TOL = 1e-15;

using namespace std;

namespace {

static const double vx[3] = { 1.0, 0.0, 0.0 };
static const double vy[3] = { 0.0, 1.0, 0.0 };
static const double vz[3] = { 0.0, 0.0, 1.0 };
static const double vt[3] = { 1.0, 1.0, 1.0 };

class NXQuaternionTest : public ::testing::Test {
protected:
        NXQuaternionTest() {
        }

        virtual void SetUp() {
        }

        virtual void TearDown() {
        }

        void expect_dvec3_equal(const double *v0, const double *v1) {
                EXPECT_GT(TOL, fabs(v0[0]-v1[0]));
                EXPECT_GT(TOL, fabs(v0[1]-v1[1]));
                EXPECT_GT(TOL, fabs(v0[2]-v1[2]));
        }

        void check_quaternion_axis_angle(const double *v) {
                nx_dvec3_copy(&vq[0], v);
                nx_quaternion_rotate(&q[0], &vq[0]);

                nx_dvec3_copy(&vR[0], v);
                nx_dmat3_transform(&R[0], &vR[0]);

                expect_dvec3_equal(&vR[0], &vq[0]);
        }

        double q[4];
        double Rq[9];
        double vq[3];
        double R[9];
        double vR[3];
};

TEST_F(NXQuaternionTest, quaternion_from_axis_angle) {
        const int n_axes = 7;
        double axis[3*n_axes] = { 1.0, 0.0, 0.0,
                                  0.0, 1.0, 0.0,
                                  0.0, 0.0, 1.0,
                                  1.0, 1.0, 0.0,
                                  0.0, 1.0, 1.0,
                                  1.0, 0.0, 1.0,
                                  1.0, 1.0, 1.0 };
        const int n_theta = 9;
        double theta[n_theta] = {
                NX_PI/6.0,
                NX_PI/4.0,
                NX_PI/2.0,
                3.0*NX_PI/4.0,
                NX_PI,
                -NX_PI/6.0,
                -NX_PI/4.0,
                -NX_PI/2.0,
                -3.0*NX_PI/4.0,
        };

        for (int i = 0; i < n_axes; ++i) {
                const double *n = axis + i*3;
                for (int j = 0; j < n_theta; ++j) {
                        nx_quaternion_from_axis_angle(&q[0], n, theta[j]);
                        nx_rotation_3d_axis_angle(&R[0], n, theta[j]);

                        check_quaternion_axis_angle(&vx[0]);
                        check_quaternion_axis_angle(&vy[0]);
                        check_quaternion_axis_angle(&vz[0]);
                        check_quaternion_axis_angle(&vt[0]);
                }
        }
}


} // namespace
