/**
 * @file tests_camera.cc
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2018 Mustafa Ozuysal. All rights reserved.
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

#include "virg/nexus/nx_camera.h"

using namespace std;

namespace {

class NXCameraTest : public ::testing::Test {
protected:
        NXCameraTest() {
                c0_ = NULL;
        }

        virtual void SetUp() {
                c0_ = nx_camera_alloc();
        }

        virtual void TearDown() {
                nx_camera_free(c0_);
        }

        struct NXCamera *c0_;
};

TEST_F(NXCameraTest, Create) {
        EXPECT_TRUE(NULL != c0_);

        const double *K = nx_camera_get_K(c0_);
        for (int i = 0; i < 9; ++i) {
                if (i == 0 || i == 4 || i == 8)
                        EXPECT_EQ(1.0, K[i]);
                else
                        EXPECT_EQ(0.0, K[i]);

        }

        const double *R = nx_camera_get_rotation(c0_);
        for (int i = 0; i < 9; ++i) {
                if (i == 0 || i == 4 || i == 8)
                        EXPECT_EQ(1.0, R[i]);
                else
                        EXPECT_EQ(0.0, R[i]);
        }

        const double *t = nx_camera_get_translation(c0_);
        for (int i = 0; i < 3; ++i) {
                EXPECT_EQ(0.0, t[i]);
        }
}

TEST_F(NXCameraTest, SetInterior) {
        nx_camera_set_interior(c0_, 2.0, 3.0, 4.0, 5.0);

        const double *K = nx_camera_get_K(c0_);
        for (int i = 0; i < 9; ++i) {
                if (i == 0)
                        EXPECT_EQ(2.0, K[i]);
                else if (i == 4)
                        EXPECT_EQ(3.0, K[i]);
                else if (i == 6)
                        EXPECT_EQ(4.0, K[i]);
                else if (i == 7)
                        EXPECT_EQ(5.0, K[i]);
                else if (i == 8)
                        EXPECT_EQ(1.0, K[i]);
                else
                        EXPECT_EQ(0.0, K[i]);

        }

        const double *R = nx_camera_get_rotation(c0_);
        for (int i = 0; i < 9; ++i) {
                if (i == 0 || i == 4 || i == 8)
                        EXPECT_EQ(1.0, R[i]);
                else
                        EXPECT_EQ(0.0, R[i]);
        }

        const double *t = nx_camera_get_translation(c0_);
        for (int i = 0; i < 3; ++i) {
                EXPECT_EQ(0.0, t[i]);
        }
}

} // namespace
