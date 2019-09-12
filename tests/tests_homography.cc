/**
 * @file tests_homography.cc
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
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_uniform_sampler.h"
#include "virg/nexus/nx_homography.h"

#define MAX(x,y) (((x) > (y)) ? (x) : (y))

#define N_TESTS 20
#define ERROR_THRESHOLD 1e-12

#define N_RANSAC_CORR 100
#define RANSAC_INLIER_RATIO 0.8
#define RANSAC_NOISE_LEVEL 1e-3
#define RANSAC_MAX_N_ITER 1000

using namespace std;

namespace {

class NXHomographyTest : public ::testing::Test {
protected:
        NXHomographyTest() {
        }

        virtual void SetUp() {
                err = 0.0;
        }

        virtual void TearDown() {
                nx_uniform_sampler_instance_free();
        }

        float *make_test_data_unit();
        struct NXPointMatch2D *make_test_data4();
        struct NXPointMatch2D *make_test_data_ransac(double *h, int n, double inlier_ratio, double noise_level);

        double measure_test_error_unit(const double *h, const float *x);
        double measure_test_error4(const double *h, const struct NXPointMatch2D *x);
        double measure_test_error(const double *h, int n, const struct NXPointMatch2D *x);

        double h[9];
        double err;
};


static inline void sample_corner(float *p, double sign_x, double sign_y)
{
        p[0] = sign_x * (NX_UNIFORM_SAMPLE_D * 0.5 + 0.5);
        p[1] = sign_y * (NX_UNIFORM_SAMPLE_D * 0.5 + 0.5);
}

float *NXHomographyTest::make_test_data_unit()
{
        float *x = NX_NEW_S(8);
        sample_corner(&x[0], -1.0, -1.0);
        sample_corner(&x[2], +1.0, -1.0);
        sample_corner(&x[4], +1.0, +1.0);
        sample_corner(&x[6], -1.0, +1.0);
        return x;
}

static inline double point_dist(const float *p, double qx, double qy)
{
        double dx = p[0] - qx;
        double dy = p[1] - qy;
        return sqrt(dx*dx+dy*dy);
}

double NXHomographyTest::measure_test_error_unit(const double *h, const float *x)
{
        double err = 0.0;
        float xp[2];
        nx_homography_transfer_fwd(h, &xp[0], &x[0]);
        err += point_dist(&xp[0], 0.0, 0.0);
        nx_homography_transfer_fwd(h, &xp[0], &x[2]);
        err += point_dist(&xp[0], 1.0, 0.0);
        nx_homography_transfer_fwd(h, &xp[0], &x[4]);
        err += point_dist(&xp[0], 1.0, 1.0);
        nx_homography_transfer_fwd(h, &xp[0], &x[6]);
        err += point_dist(&xp[0], 0.0, 1.0);
        return err;
}

struct NXPointMatch2D *NXHomographyTest::make_test_data4()
{
        struct NXPointMatch2D *corr = NX_NEW(4, struct NXPointMatch2D);
        sample_corner(&corr[0].x[0], -1.0, -1.0);
        sample_corner(&corr[1].x[0], +1.0, -1.0);
        sample_corner(&corr[2].x[0], +1.0, +1.0);
        sample_corner(&corr[3].x[0], -1.0, +1.0);

        sample_corner(&corr[0].xp[0], -1.0, -1.0);
        sample_corner(&corr[1].xp[0], +1.0, -1.0);
        sample_corner(&corr[2].xp[0], +1.0, +1.0);
        sample_corner(&corr[3].xp[0], -1.0, +1.0);
        return corr;
}

double NXHomographyTest::measure_test_error4(const double *h, const struct NXPointMatch2D *corr)
{
        double err = 0.0;
        float xp[2];
        for (int i = 0; i < 4; ++i) {
                nx_homography_transfer_fwd(h, &xp[0], &corr[i].x[0]);
                err = MAX(err, point_dist(&xp[0], corr[i].xp[0], corr[i].xp[1]));
        }
        return err;
}

struct NXPointMatch2D *NXHomographyTest::make_test_data_ransac(double *h, int n, double inlier_ratio, double noise_level)
{
        float *h_data = make_test_data_unit();
        double hd[8];
        for (int i = 0; i < 8; ++i)
                hd[i] = h_data[i];
        nx_homography_estimate_unit(h, &hd[0]);
        nx_free(h_data);

        struct NXPointMatch2D *corr = NX_NEW(n, struct NXPointMatch2D);
        for (int i = 0; i < n; ++i) {
                double sign_x = NX_UNIFORM_SAMPLE_D > 0.5 ? +1.0 : -1.0;
                double sign_y = NX_UNIFORM_SAMPLE_D > 0.5 ? +1.0 : -1.0;
                sample_corner(&corr[i].x[0], sign_x, sign_y);

                if (NX_UNIFORM_SAMPLE_D < inlier_ratio) {
                        corr[i].is_inlier = NX_TRUE;
                        nx_homography_transfer_fwd(h, &corr[i].xp[0], &corr[i].x[0]);
                        corr[i].xp[0] += (NX_UNIFORM_SAMPLE_D - 0.5) * noise_level;
                        corr[i].xp[1] += (NX_UNIFORM_SAMPLE_D - 0.5) * noise_level;
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

TEST_F(NXHomographyTest, unit) {
        for (int i = 0; i < N_TESTS; ++i) {
                float *test_data = make_test_data_unit();
                double td[8];
                for (int i = 0; i < 8; ++i)
                        td[i] = test_data[i];
                nx_homography_estimate_unit(h, &td[0]);
                err = MAX(err, measure_test_error_unit(h, test_data));
                nx_free(test_data);
        }

        EXPECT_GT(ERROR_THRESHOLD, fabs(err));
}

TEST_F(NXHomographyTest, dlt4) {
        int indices[4] = { 0, 1, 2, 3 };
        for (int i = 0; i < N_TESTS; ++i) {
                struct NXPointMatch2D *test_data = make_test_data4();
                nx_homography_estimate_4pt(h, indices, test_data);
                err = MAX(err, measure_test_error4(h, test_data));
                nx_free(test_data);
        }

        EXPECT_GT(ERROR_THRESHOLD, fabs(err));
}

TEST_F(NXHomographyTest, dlt) {
        for (int i = 0; i < N_TESTS; ++i) {
                struct NXPointMatch2D *test_data = make_test_data4();
                nx_homography_estimate_dlt(h, 4, test_data);
                err = MAX(err, measure_test_error4(h, test_data));
                nx_free(test_data);
        }

        EXPECT_GT(ERROR_THRESHOLD, fabs(err));
}

TEST_F(NXHomographyTest, ransac) {
        for (int i = 0; i < N_TESTS; ++i) {
                double h_gt[9];
                struct NXPointMatch2D *test_data = make_test_data_ransac(h_gt, N_RANSAC_CORR, RANSAC_INLIER_RATIO, RANSAC_NOISE_LEVEL);

                nx_homography_estimate_norm_ransac(h, N_RANSAC_CORR, test_data, RANSAC_NOISE_LEVEL * 2.0, RANSAC_MAX_N_ITER);

                struct NXPointMatch2D gt_data[4];
                gt_data[0].x[0] = 0.0; gt_data[0].x[1] = 0.0;
                gt_data[1].x[0] = 1.0; gt_data[1].x[1] = 0.0;
                gt_data[2].x[0] = 1.0; gt_data[2].x[1] = 1.0;
                gt_data[3].x[0] = 0.0; gt_data[3].x[1] = 1.0;
                for (int j = 0; j < 4; ++j)
                        nx_homography_transfer_fwd(h_gt, &gt_data[j].xp[0],
                                                   &gt_data[j].x[0]);
                err = MAX(err, measure_test_error4(h, gt_data));
                nx_free(test_data);
        }

        EXPECT_GT(RANSAC_NOISE_LEVEL * 2.0, fabs(err));
}

} // namespace
