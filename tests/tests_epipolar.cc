/**
 * @file tests_epipolar.cc
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2019 Mustafa Ozuysal. All rights reserved.
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
#include "virg/nexus/nx_mat234.h"
#include "virg/nexus/nx_uniform_sampler.h"
#include "virg/nexus/nx_epipolar.h"

#define ABS_MAX(x,y) ((fabs(x) > fabs(y)) ? fabs(x) : fabs(y))

#define N_TESTS 100
#define ERROR_THRESHOLD 1e-9

#define N_RANSAC_CORR 100
#define RANSAC_INLIER_RATIO 0.7
#define RANSAC_NOISE_LEVEL 1e-3
#define RANSAC_MAX_N_ITER 200

using namespace std;

namespace {

class NXEpipolarTest : public ::testing::Test {
protected:
        NXEpipolarTest() {
        }

        virtual void SetUp() {
                err = 0.0;
        }

        virtual void TearDown() {
                nx_uniform_sampler_instance_free();
        }

        struct NXPointMatch2D *make_test_data8();
        struct NXPointMatch2D *make_test_data16();
        struct NXPointMatch2D *make_test_data_ransac(int n, double inlier_ratio, double noise_level);

        double measure_test_error(const double *f, int n,
                                  const struct NXPointMatch2D *x,
                                  bool only_inliers = false);

        double f[9];
        double err;
};


static inline void sample_corner(float *p, double sign_x, double sign_y)
{
        p[0] = sign_x * (NX_UNIFORM_SAMPLE_D * 0.5 + 0.5);
        p[1] = sign_y * (NX_UNIFORM_SAMPLE_D * 0.5 + 0.5);
}


static inline double point_dist(const float *p, double qx, double qy)
{
        double dx = p[0] - qx;
        double dy = p[1] - qy;
        return sqrt(dx*dx+dy*dy);
}

struct NXPointMatch2D *NXEpipolarTest::make_test_data8()
{
        struct NXPointMatch2D *corr = NX_NEW(8, struct NXPointMatch2D);
        sample_corner(&corr[0].x[0], -1.0, -1.0);
        sample_corner(&corr[1].x[0], +1.0, -1.0);
        sample_corner(&corr[2].x[0], +1.0, +1.0);
        sample_corner(&corr[3].x[0], -1.0, +1.0);
        sample_corner(&corr[4].x[0],  0.0, -1.0);
        sample_corner(&corr[5].x[0], +1.0,  0.0);
        sample_corner(&corr[6].x[0],  0.0, +1.0);
        sample_corner(&corr[7].x[0], -1.0,  0.0);

        sample_corner(&corr[0].xp[0], -1.0, -1.0);
        sample_corner(&corr[1].xp[0], +1.0, -1.0);
        sample_corner(&corr[2].xp[0], +1.0, +1.0);
        sample_corner(&corr[3].xp[0], -1.0, +1.0);
        sample_corner(&corr[4].xp[0],  0.0, -1.0);
        sample_corner(&corr[5].xp[0], +1.0,  0.0);
        sample_corner(&corr[6].xp[0],  0.0, +1.0);
        sample_corner(&corr[7].xp[0], -1.0,  0.0);

        for (int i = 0; i < 8; ++i)
                corr[i].xp[1] = corr[i].x[1];

        return corr;
}

struct NXPointMatch2D *NXEpipolarTest::make_test_data16()
{
        struct NXPointMatch2D *corr = NX_NEW(16, struct NXPointMatch2D);
        sample_corner(&corr[0].x[0], -1.0, -1.0);
        sample_corner(&corr[1].x[0], +1.0, -1.0);
        sample_corner(&corr[2].x[0], +1.0, +1.0);
        sample_corner(&corr[3].x[0], -1.0, +1.0);
        sample_corner(&corr[4].x[0],  0.0, -1.0);
        sample_corner(&corr[5].x[0], +1.0,  0.0);
        sample_corner(&corr[6].x[0],  0.0, +1.0);
        sample_corner(&corr[7].x[0], -1.0,  0.0);
        sample_corner(&corr[8].x[0], -1.0, -1.0);
        sample_corner(&corr[9].x[0], +1.0, -1.0);
        sample_corner(&corr[10].x[0], +1.0, +1.0);
        sample_corner(&corr[11].x[0], -1.0, +1.0);
        sample_corner(&corr[12].x[0],  0.0, -1.0);
        sample_corner(&corr[13].x[0], +1.0,  0.0);
        sample_corner(&corr[14].x[0],  0.0, +1.0);
        sample_corner(&corr[15].x[0], -1.0,  0.0);

        sample_corner(&corr[0].xp[0], -1.0, -1.0);
        sample_corner(&corr[1].xp[0], +1.0, -1.0);
        sample_corner(&corr[2].xp[0], +1.0, +1.0);
        sample_corner(&corr[3].xp[0], -1.0, +1.0);
        sample_corner(&corr[4].xp[0],  0.0, -1.0);
        sample_corner(&corr[5].xp[0], +1.0,  0.0);
        sample_corner(&corr[6].xp[0],  0.0, +1.0);
        sample_corner(&corr[7].xp[0], -1.0,  0.0);
        sample_corner(&corr[8].xp[0], -1.0, -1.0);
        sample_corner(&corr[9].xp[0], +1.0, -1.0);
        sample_corner(&corr[10].xp[0], +1.0, +1.0);
        sample_corner(&corr[11].xp[0], -1.0, +1.0);
        sample_corner(&corr[12].xp[0],  0.0, -1.0);
        sample_corner(&corr[13].xp[0], +1.0,  0.0);
        sample_corner(&corr[14].xp[0],  0.0, +1.0);
        sample_corner(&corr[15].xp[0], -1.0,  0.0);

        for (int i = 0; i < 16; ++i)
                corr[i].xp[1] = corr[i].x[1];

        return corr;
}

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
                                                             double noise_level)
{
        struct NXPointMatch2D *corr = NX_NEW(n, struct NXPointMatch2D);
        for (int i = 0; i < n; ++i) {
                double sign_x = NX_UNIFORM_SAMPLE_D > 0.5 ? +1.0 : -1.0;
                double sign_y = NX_UNIFORM_SAMPLE_D > 0.5 ? +1.0 : -1.0;
                sample_corner(&corr[i].x[0], sign_x, sign_y);

                if (NX_UNIFORM_SAMPLE_D < inlier_ratio) {
                        sample_corner(&corr[i].xp[0], sign_x, sign_y);
                        corr[i].is_inlier = NX_TRUE;
                        corr[i].xp[1] = corr[i].x[1] + (NX_UNIFORM_SAMPLE_D - 0.5) * noise_level;
                        corr[i].match_cost = NX_UNIFORM_SAMPLE_S * 20.0;
                } else {
                        corr[i].is_inlier = NX_FALSE;
                        corr[i].xp[0] = NX_UNIFORM_SAMPLE_D * 2.0 - 1.0;
                        corr[i].xp[1] = NX_UNIFORM_SAMPLE_D * 2.0 - 1.0;
                        corr[i].match_cost = NX_UNIFORM_SAMPLE_S * 30.0 + 5.0;
                }
        }

        return corr;
}


TEST_F(NXEpipolarTest, eight_points) {
        int indices[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
        for (int i = 0; i < N_TESTS; ++i) {
                struct NXPointMatch2D *test_data = make_test_data8();
                nx_fundamental_estimate_8pt(f, indices, test_data);
                err = ABS_MAX(err, measure_test_error(f, 8, test_data));
                nx_free(test_data);
        }

        // NX_LOG(NX_LOG_TAG, "Max error is = %.4g", err);
        EXPECT_GT(ERROR_THRESHOLD, err);
}


TEST_F(NXEpipolarTest, n_points) {
        for (int i = 0; i < N_TESTS; ++i) {
                struct NXPointMatch2D *test_data = make_test_data16();
                nx_fundamental_estimate(f, 16, test_data);
                err = ABS_MAX(err, measure_test_error(f, 16, test_data));
                nx_free(test_data);
        }

        // NX_LOG(NX_LOG_TAG, "Max error is = %.4g", err);
        EXPECT_GT(ERROR_THRESHOLD, err);
}

TEST_F(NXEpipolarTest, ransac) {
        for (int i = 0; i < N_TESTS; ++i) {
                struct NXPointMatch2D *test_data = make_test_data_ransac(N_RANSAC_CORR, RANSAC_INLIER_RATIO, RANSAC_NOISE_LEVEL);
                nx_fundamental_estimate_norm_ransac(f, N_RANSAC_CORR, test_data, RANSAC_NOISE_LEVEL * 2.0, RANSAC_MAX_N_ITER);
                err = ABS_MAX(err, measure_test_error(f, N_RANSAC_CORR, test_data, true));
                nx_free(test_data);
        }

        // NX_LOG(NX_LOG_TAG, "Max error is = %.4g", err);
        EXPECT_GT(RANSAC_NOISE_LEVEL * 2.0, err);
}

} // namespace
