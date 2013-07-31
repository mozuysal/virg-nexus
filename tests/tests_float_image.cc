/**
 * @file tests_float_image.cc
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Izmir Institute of Technology. All rights reserved.
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

#include "virg/nexus/nx_float_image.h"
#include "virg/nexus/nx_image.h"

using namespace std;

namespace {

static const int NX = 4;
static const int NY = 4;
static const int N = NX*NY;
static const uchar TEST_IMAGE_DATA[N] = { 10, 20 ,30, 40,
                                          50, 60, 70, 80,
                                          90, 100, 110, 120,
                                          130, 140, 150, 160 };

static const uchar TEST_IMAGE_COLOR_DATA[N*4] = { 10, 0, 160, 255, 20, 0, 150, 255, 30, 0, 140, 255, 40, 0, 130, 255,
                                                  50, 0, 120, 255, 60, 0, 110, 255, 70, 0, 100, 255, 80, 0, 90, 255,
                                                  90, 0, 80, 255, 100, 0, 70, 255, 110, 0, 60, 255, 120, 0, 50, 255,
                                                  130, 0, 40, 255, 140, 0, 30, 255, 150, 0, 20, 255, 160, 0, 10, 255 };

static const char TMP_RAW_IMAGE_FILENAME[] = "/tmp/nx_tmp_image.raw";

static const float TEST_SIGMA = 1.1f;

class NXFloatImageTest : public ::testing::Test {
protected:
    NXFloatImageTest()
    {
        img0_ = NULL;
        img1_ = NULL;

        fimg0_ = NULL;
        fimg1_ = NULL;
    }

    virtual void SetUp()
    {
            img0_ = nx_image_new_gray(NX, NY);
            memcpy(img0_->data, TEST_IMAGE_DATA, sizeof(TEST_IMAGE_DATA));

            img1_ = nx_image_new_rgba(NX, NY);
            memcpy(img1_->data, TEST_IMAGE_COLOR_DATA, sizeof(TEST_IMAGE_COLOR_DATA));
    }

    virtual void TearDown()
    {
            nx_image_free(img0_);
            nx_image_free(img1_);
    }

    struct NXImage *img0_;
    struct NXImage *img1_;

    struct NXFloatImage *fimg0_;
    struct NXFloatImage *fimg1_;
};

TEST_F(NXFloatImageTest, FloatImageAllocFree) {
    fimg0_ = nx_float_image_alloc();
    EXPECT_TRUE(NULL != fimg0_);
    EXPECT_TRUE(NULL != fimg0_->mem);
    EXPECT_EQ(NULL, fimg0_->data);
    EXPECT_EQ(0, fimg0_->width);
    EXPECT_EQ(0, fimg0_->height);
    nx_float_image_free(fimg0_);
}

TEST_F(NXFloatImageTest, FloatImageNewGrayFree) {
    fimg0_ = nx_float_image_new_gray(640, 480);
    EXPECT_TRUE(NULL != fimg0_);
    EXPECT_TRUE(NULL != fimg0_->mem);
    EXPECT_TRUE(NULL != fimg0_->data);
    EXPECT_EQ(640, fimg0_->width);
    EXPECT_EQ(480, fimg0_->height);
    EXPECT_EQ(1, fimg0_->n_channels);
    nx_float_image_free(fimg0_);
}

TEST_F(NXFloatImageTest, FloatImageNewRGBAFree) {
    fimg0_ = nx_float_image_new_rgba(640, 480);
    EXPECT_TRUE(NULL != fimg0_);
    EXPECT_TRUE(NULL != fimg0_->mem);
    EXPECT_TRUE(NULL != fimg0_->data);
    EXPECT_EQ(640, fimg0_->width);
    EXPECT_EQ(480, fimg0_->height);
    EXPECT_EQ(4, fimg0_->n_channels);
    nx_float_image_free(fimg0_);
}

TEST_F(NXFloatImageTest, FloatImageGrayFromUChar) {
    fimg0_ = nx_float_image_from_uchar0(img0_);
    for (int i = 0; i < N; ++i)
        EXPECT_FLOAT_EQ(TEST_IMAGE_DATA[i] / 255.0f, fimg0_->data[i]);
    nx_float_image_free(fimg0_);
}

TEST_F(NXFloatImageTest, FloatImageRGBAFromUChar) {
    fimg0_ = nx_float_image_from_uchar0(img1_);
    for (int i = 0; i < 4*N; ++i)
        EXPECT_FLOAT_EQ(TEST_IMAGE_COLOR_DATA[i] / 255.0f, fimg0_->data[i]);
    nx_float_image_free(fimg0_);
}

TEST_F(NXFloatImageTest, FloatImageGraySaveLoadAsIs) {
    fimg0_ = nx_float_image_from_uchar0(img0_);
    nx_float_image_xsave_raw(fimg0_, TMP_RAW_IMAGE_FILENAME);
    fimg1_ = nx_float_image_alloc();
    nx_float_image_xload_raw(fimg1_, TMP_RAW_IMAGE_FILENAME);
    for (int i = 0; i < N; ++i)
        EXPECT_EQ(fimg0_->data[i], fimg1_->data[i]);
    nx_float_image_free(fimg0_);
    nx_float_image_free(fimg1_);
}

TEST_F(NXFloatImageTest, FloatImageRGBASaveLoadAsIs) {
    fimg0_ = nx_float_image_from_uchar0(img1_);
    nx_float_image_xsave_raw(fimg0_, TMP_RAW_IMAGE_FILENAME);
    fimg1_ = nx_float_image_alloc();
    nx_float_image_xload_raw(fimg1_, TMP_RAW_IMAGE_FILENAME);
    for (int i = 0; i < 4*N; ++i)
        EXPECT_EQ(fimg0_->data[i], fimg1_->data[i]);
    nx_float_image_free(fimg0_);
    nx_float_image_free(fimg1_);
}

TEST_F(NXFloatImageTest, FloatImageGrayFilterS) {
    fimg0_ = nx_float_image_from_uchar0(img0_);
    nx_float_image_smooth_s(fimg0_, fimg0_, TEST_SIGMA, TEST_SIGMA, NULL);
    nx_float_image_free(fimg0_);
}




} // namespace
