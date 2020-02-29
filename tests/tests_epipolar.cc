/**
 * @file tests_epipolar.cc
 *
 * Copyright (C) 2019,2020 Mustafa Ozuysal. All rights reserved.
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
#include "virg/nexus/nx_pinhole.h"
#include "virg/nexus/nx_epipolar.h"

#define ABS_MAX(x,y) ((fabs(x) > fabs(y)) ? fabs(x) : fabs(y))

extern bool IS_VALGRIND_RUN;
static int N_TESTS = 20;
#define ERROR_THRESHOLD 1e-4

#define N_RANSAC_CORR 100
#define RANSAC_INLIER_RATIO 0.7
#define RANSAC_NOISE_LEVEL 1e-3
#define RANSAC_MAX_N_ITER 200

static const int N_POINTS_3D = 1000;

using namespace std;

namespace {

class NXEpipolarTest : public ::testing::Test {
protected:
        NXEpipolarTest() {
                if (IS_VALGRIND_RUN)
                        N_TESTS = 2;
        }

        virtual void SetUp() {
                sampler = nx_uniform_sampler_alloc();
                nx_uniform_sampler_init_time(sampler);

                err = 0.0;
                X = gen_points_3d(N_POINTS_3D);
        }

        virtual void TearDown() {
                nx_uniform_sampler_free(sampler);
                nx_free(X);
        }

         float *gen_points_3d(int n) {
                float *X = NX_NEW_S(3*n);
                for (int i = 0; i < n; ++i) {
                        X[i*3+0] = nx_uniform_sampler_sample_s(sampler)*2.0f - 1.0f;
                        X[i*3+1] = nx_uniform_sampler_sample_s(sampler)*2.0f - 1.0f;
                        X[i*3+2] = 5.0;
                }
                return X;
        }

        struct NXPointMatch2D *gen_matches(int n,
                                           const double *R, const double *t) {
                double P0[12];
                double P1[12];
                nx_pinhole_projection_from_krt(P0, NULL, NULL, NULL);
                nx_pinhole_projection_from_krt(P1, NULL, &R[0], &t[0]);

                struct NXPointMatch2D *corr_list = NX_NEW(n,
                                                          struct NXPointMatch2D);
                for (int i = 0; i < n; ++i) {
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

        struct NXPointMatch2D *make_test_data_ransac(int n, double inlier_ratio,
                                                     double noise_level,
                                                     const double *R,
                                                     const double *t);

        double measure_test_error(const double *f, int n,
                                  const struct NXPointMatch2D *x,
                                  bool only_inliers = false);

        void check_decomposition_error(const double *E,
                                       const double *R,
                                       const double *t);
        void check_z_test(const double *E, const double *R, const double *t);

        struct NXUniformSampler *sampler;

        float* X;
        double f[9];
        double err;
};

double NXEpipolarTest::measure_test_error(const double *f, int n,
                                          const struct NXPointMatch2D *corr,
                                          bool only_inliers)
{
        double err = 0.0;
        for (int i = 0; i < n; ++i) {
                if (!only_inliers || corr[i].is_inlier) {
                        double x[2]  = { corr[i].x[0], corr[i].x[1] };
                        double xp[2] = { corr[i].xp[0], corr[i].xp[1] };
                        double e_i = nx_dmat3_xptFx(f, &x[0], &xp[0]);
                        err = ABS_MAX(err, e_i);
                }
        }

        return err;
}


struct NXPointMatch2D *NXEpipolarTest::make_test_data_ransac(int n,
                                                             double inlier_ratio,
                                                             double noise_level,
                                                             const double *R,
                                                             const double *t)
{
        EXPECT_LT(n, N_POINTS_3D);

        struct NXPointMatch2D *corr = gen_matches(n, R, t);
        for (int i = 0; i < n; ++i) {
                if (NX_UNIFORM_SAMPLE_D >= inlier_ratio) {
                        corr[i].is_inlier = NX_FALSE;
                        corr[i].xp[0] += nx_uniform_sampler_sample64(sampler) * 2.0 - 1.0;
                        corr[i].xp[1] += nx_uniform_sampler_sample64(sampler) * 2.0 - 1.0;;
                        corr[i].match_cost = NX_UNIFORM_SAMPLE_S * 30.0 + 5.0;
                } else {
                        corr[i].xp[0] += (nx_uniform_sampler_sample64(sampler) - 0.5) * noise_level;
                        corr[i].xp[1] += (nx_uniform_sampler_sample64(sampler) - 0.5) * noise_level;
                        corr[i].match_cost = NX_UNIFORM_SAMPLE_S * 20.0;
                }
        }

        return corr;
}

void NXEpipolarTest::check_decomposition_error(const double *E,
                                               const double *R,
                                               const double *t)
{
        double *Re[4];
        double *te[4];
        for (int i = 0; i < 4; ++i) {
                Re[i] = NX_NEW_D(9);
                te[i] = NX_NEW_D(3);
        }

        // nx_dmat3_print(&R[0], "R");
        nx_essential_decompose_to_Rt(&E[0], &Re[0], &te[0]);
        double re_min = DBL_MAX;
        double te_min = DBL_MAX;
        for (int i = 0; i < 4; ++i) {
                // nx_dmat3_print(&Re[i][0], "Re_i");
                nx_dmat3_sub(&Re[i][0], &R[0]);

                double e = nx_dmat3_frob_norm(&Re[i][0]);
                if (e < re_min)
                        re_min = e;

                e = 0.0;
                for (int j = 0; j < 3; ++j) {
                        double d = t[j] - te[i][j];
                        e += d*d;
                }
                e = sqrt(e);
                if (e < te_min)
                        te_min = e;
        }


        // NX_LOG(NX_LOG_TAG, "min rotation    error norm = %.2e", re_min);
        // NX_LOG(NX_LOG_TAG, "min translation error norm = %.2e", te_min);

        EXPECT_GT(1e-15, re_min);
        EXPECT_GT(1e-15, te_min);

        for (int i = 0; i < 4; ++i) {
                nx_free(Re[i]);
                nx_free(te[i]);
        }
}

void NXEpipolarTest::check_z_test(const double *E,
                                  const double *R,
                                  const double *t)
{
        const int N = 50;
        struct NXPointMatch2D *corr_list = gen_matches(N, &R[0], &t[0]);

        double *Re[4];
        double *te[4];
        for (int i = 0; i < 4; ++i) {
                Re[i] = NX_NEW_D(9);
                te[i] = NX_NEW_D(3);
        }

        // NX_LOG(NX_LOG_TAG, "------------------------------------------------");
        nx_essential_decompose_to_Rt(&E[0], &Re[0], &te[0]);
        int sum_best = -2*N;
        int i_best = nx_epipolar_pick_best_Rt(&Re[0], &te[0],
                                              N, corr_list, &sum_best);
        // NX_LOG(NX_LOG_TAG, "------------------------------------------------");
        // nx_dmat3_print(&R[0], "R");
        // nx_dvec3_print(&t[0], "t");
        // NX_LOG(NX_LOG_TAG, "------------------------------------------------");
        // nx_dmat3_print(&Re[i_best][0], "Rbest");
        // nx_dvec3_print(&te[i_best][0], "tbest");

        nx_dmat3_sub(&Re[i_best][0], &R[0]);
        double er = nx_dmat3_frob_norm(&Re[i_best][0]);
        double et = 0.0;
        for (int j = 0; j < 3; ++j) {
                double d = t[j] - te[i_best][j];
                et += d*d;
        }
        et = sqrt(et);

        // NX_LOG(NX_LOG_TAG, "min rotation    error norm = %.2e", er);
        // NX_LOG(NX_LOG_TAG, "min translation error norm = %.2e", et);

        EXPECT_GT(1e-14, er);
        EXPECT_GT(1e-14, et);

        for (int i = 0; i < 4; ++i) {
                nx_free(Re[i]);
                nx_free(te[i]);
        }
        nx_free(corr_list);
}

TEST_F(NXEpipolarTest, eight_points) {
        double t[3] = { 1.0, 4.0, -2.0 };
        double R[9];

        int indices[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
        for (int i = 0; i < N_TESTS; ++i) {
                double alpha = nx_uniform_sampler_sample64(sampler)*NX_PI/4-NX_PI/8;
                double beta  = nx_uniform_sampler_sample64(sampler)*NX_PI/4-NX_PI/8;
                double gamma = nx_uniform_sampler_sample64(sampler)*NX_PI/4-NX_PI/8;
                nx_rotation_3d_RxRyRz(&R[0], alpha, beta, gamma);

                struct NXPointMatch2D *test_data = gen_matches(8, &R[0], &t[0]);
                nx_fundamental_estimate_8pt(f, indices, test_data);
                err = ABS_MAX(err, measure_test_error(f, 8, test_data));
                nx_free(test_data);
        }

        // NX_LOG(NX_LOG_TAG, "Max error is = %.4g", err);
        EXPECT_GT(ERROR_THRESHOLD, err);
}


TEST_F(NXEpipolarTest, n_points) {
        double t[3] = { 1.0, 4.0, -2.0 };
        double R[9];

        for (int i = 0; i < N_TESTS; ++i) {
                double alpha = nx_uniform_sampler_sample64(sampler)*NX_PI/4-NX_PI/8;
                double beta  = nx_uniform_sampler_sample64(sampler)*NX_PI/4-NX_PI/8;
                double gamma = nx_uniform_sampler_sample64(sampler)*NX_PI/4-NX_PI/8;
                nx_rotation_3d_RxRyRz(&R[0], alpha, beta, gamma);

                struct NXPointMatch2D *test_data = gen_matches(16, &R[0], &t[0]);
                nx_fundamental_estimate(f, 16, test_data);
                err = ABS_MAX(err, measure_test_error(f, 16, test_data));
                nx_free(test_data);
        }

        // NX_LOG(NX_LOG_TAG, "Max error is = %.4g", err);
        EXPECT_GT(ERROR_THRESHOLD, err);
}

TEST_F(NXEpipolarTest, ransac) {
        double t[3] = { 1.0, 4.0, -2.0 };
        double R[9];

        for (int i = 0; i < N_TESTS; ++i) {
                double alpha = nx_uniform_sampler_sample64(sampler)*NX_PI/4-NX_PI/8;
                double beta  = nx_uniform_sampler_sample64(sampler)*NX_PI/4-NX_PI/8;
                double gamma = nx_uniform_sampler_sample64(sampler)*NX_PI/4-NX_PI/8;
                nx_rotation_3d_RxRyRz(&R[0], alpha, beta, gamma);

                struct NXPointMatch2D *test_data = make_test_data_ransac(N_RANSAC_CORR, RANSAC_INLIER_RATIO,
                                                                         RANSAC_NOISE_LEVEL, &R[0], &t[0]);
                nx_fundamental_estimate_norm_ransac(f, N_RANSAC_CORR, test_data, RANSAC_NOISE_LEVEL * 2.0, RANSAC_MAX_N_ITER);
                err = ABS_MAX(err, measure_test_error(f, N_RANSAC_CORR, test_data, true));
                nx_free(test_data);
        }

        // NX_LOG(NX_LOG_TAG, "Max error is = %.4g", err);
        EXPECT_GT(RANSAC_NOISE_LEVEL * 2.0, err);
}

TEST_F(NXEpipolarTest, essential_decompose) {
        double E[9];
        double R[9];
        double t[3];

        t[0] = -1.0/sqrt(3.0);
        t[1] = -1.0/sqrt(3.0);
        t[2] =  1.0/sqrt(3.0);

        for (int alpha = -1; alpha < 2; alpha++) {
                for (int beta = -1; beta < 2; beta++) {
                        for (int gamma = -1; gamma < 2; gamma++) {
                                nx_rotation_3d_RxRyRz(&R[0],
                                                      alpha*NX_PI/4,
                                                      beta*NX_PI/6,
                                                      gamma*NX_PI/3);
                                nx_essential_from_Rt(&E[0], &R[0], &t[0]);
                                check_decomposition_error(&E[0], &R[0], &t[0]);
                        }
                }
        }
}

TEST_F(NXEpipolarTest, essential_z_test) {
        double E[9];
        double R[9];
        double t[3];

        t[0] = -1.0/sqrt(3.0);
        t[1] = -1.0/sqrt(3.0);
        t[2] =  1.0/sqrt(3.0);

        double INCREMENT = IS_VALGRIND_RUN ? 1.0 : 0.1;

        for (double alpha = -1.0; alpha < 1.001; alpha += INCREMENT) {
                for (double beta = -1.0; beta < 1.001; beta += INCREMENT) {
                        for (double gamma = -1.0; gamma < 1.001; gamma += INCREMENT) {
                                nx_rotation_3d_RxRyRz(&R[0],
                                                      alpha*NX_PI/6,
                                                      beta*NX_PI/6,
                                                      gamma*NX_PI/6);
                                nx_essential_from_Rt(&E[0], &R[0], &t[0]);
                                check_z_test(&E[0], &R[0], &t[0]);
                        }
                }
        }
}


} // namespace
