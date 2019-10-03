/**
 * @file tests_triangulation.cc
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
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_vec234.h"
#include "virg/nexus/nx_mat234.h"
#include "virg/nexus/nx_math.h"
#include "virg/nexus/nx_uniform_sampler.h"
#include "virg/nexus/nx_rotation_3d.h"
#include "virg/nexus/nx_epipolar.h"
#include "virg/nexus/nx_pinhole.h"
#include "virg/nexus/nx_triangulation.h"

using namespace std;

static const int N_POINTS = 100;

namespace {

class NXTriangulationTest : public ::testing::Test {
protected:
        NXTriangulationTest() {
        }

        virtual void SetUp() {
                sampler = nx_uniform_sampler_alloc();
                nx_uniform_sampler_init_time(sampler);

                P0 = NX_NEW_D(12);
                P1 = NX_NEW_D(12);
        }

        virtual void TearDown() {
                nx_uniform_sampler_free(sampler);

                nx_free(P0);
                nx_free(P1);
        }

        float *gen_points_3d() {
                float *X = NX_NEW_S(3*N_POINTS);
                for (int i = 0; i < N_POINTS; ++i) {
                        X[i*3+0] = nx_uniform_sampler_sample_s(sampler)*2.0f - 1.0f;
                        X[i*3+1] = nx_uniform_sampler_sample_s(sampler)*2.0f - 1.0f;
                        X[i*3+2] = 2.0;
                }
                return X;
        }

        struct NXPointMatch2D *gen_matches(const float *X) {
                struct NXPointMatch2D *corr_list = NX_NEW(N_POINTS,
                                                          struct NXPointMatch2D);
                for (int i = 0; i < N_POINTS; ++i) {
                        struct NXPointMatch2D *pm = corr_list + i;
                        nx_pinhole_project(P0, 1, &X[i*3], &pm->x[0]);
                        nx_pinhole_project(P1, 1, &X[i*3], &pm->xp[0]);
                        pm->match_cost = 0.0f;
                        pm->sigma_x = 0.0f;
                        pm->sigma_xp = 0.0f;
                        pm->id = 0;
                        pm->idp = 0;
                        pm->is_inlier = NX_TRUE;
                }

                return corr_list;
        }

        void check_reconstruction(const float *X, struct NXRPoint3D *pt_list) {
                for (int i = 0; i < N_POINTS; ++i) {
                        struct NXRPoint3D *pt = pt_list + i;
                        nx_svec4h_to_euclidean(&pt->Xh[0]);
                        double e = sqrt(nx_svec3_dist_sq(&pt->Xh[0], X + 3*i));
                        // NX_LOG(NX_LOG_TAG, "%03d: e = %.2e", e);

                        EXPECT_GT(1e-5, e);
                }
        }

        struct NXUniformSampler *sampler;
        double *P0;
        double *P1;
};

TEST_F(NXTriangulationTest, rectified_test) {
        float *X = gen_points_3d();

        double t[3] = { 1.0, 0.0, 0.0 };
        nx_pinhole_projection_from_krt(P0, NULL, NULL, NULL);
        // nx_dmat34_print(P0, "P0");
        nx_pinhole_projection_from_krt(P1, NULL, NULL, &t[0]);
        // nx_dmat34_print(P1, "P1");
        struct NXPointMatch2D *corr_list = gen_matches(X);

        struct NXRPoint3D *pt_list = NX_NEW(N_POINTS, struct NXRPoint3D);
        int n = nx_triangulate(N_POINTS, pt_list, N_POINTS, corr_list, P0, P1);
        EXPECT_EQ(N_POINTS, n);

        check_reconstruction(X, pt_list);

        nx_free(pt_list);
        nx_free(corr_list);
        nx_free(X);
}

TEST_F(NXTriangulationTest, generic_test) {
        for (int i = 0; i < 10; ++i) {
                float *X = gen_points_3d();

                double t[3] = { 1.0, 4.0, -2.0 };
                nx_pinhole_projection_from_krt(P0, NULL, NULL, NULL);
                // nx_dmat34_print(P0, "P0");

                double R[9];
                double alpha = nx_uniform_sampler_sample64(sampler)*NX_PI/4-NX_PI/8;
                double beta  = nx_uniform_sampler_sample64(sampler)*NX_PI/4-NX_PI/8;
                double gamma = nx_uniform_sampler_sample64(sampler)*NX_PI/4-NX_PI/8;
                nx_rotation_3d_RxRyRz(&R[0], alpha, beta, gamma);
                nx_pinhole_projection_from_krt(P1, NULL, &R[0], &t[0]);
                // nx_dmat34_print(P1, "P1");
                struct NXPointMatch2D *corr_list = gen_matches(X);

                struct NXRPoint3D *pt_list = NX_NEW(N_POINTS, struct NXRPoint3D);
                int n = nx_triangulate(N_POINTS, pt_list, N_POINTS, corr_list, P0, P1);
                EXPECT_EQ(N_POINTS, n);

                check_reconstruction(X, pt_list);

                nx_free(pt_list);
                nx_free(corr_list);
                nx_free(X);
        }
}

} // namespace
