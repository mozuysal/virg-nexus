/**
 * @file tests_image.cc
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

static const char TMP_PGM_IMAGE_FILENAME[] = "/tmp/nx_tmp_image.pgm";
static const char TMP_PPM_IMAGE_FILENAME[] = "/tmp/nx_tmp_image.ppm";

static const float TEST_SIGMA = 1.1f;

class NXImageTest : public ::testing::Test {
protected:
    NXImageTest()
    {
        img0_ = NULL;
        img1_ = NULL;
    }

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }

    struct NXImage *img0_;
    struct NXImage *img1_;
};

TEST_F(NXImageTest, ImageAllocFree) {
    img0_ = nx_image_alloc();
    EXPECT_TRUE(NULL != img0_);
    EXPECT_TRUE(NULL != img0_->mem);
    EXPECT_EQ(NULL, img0_->data);
    EXPECT_EQ(0, img0_->width);
    EXPECT_EQ(0, img0_->height);
    nx_image_free(img0_);
}

TEST_F(NXImageTest, ImageNewGrayFree) {
    img0_ = nx_image_new_gray(640, 480);
    EXPECT_TRUE(NULL != img0_);
    EXPECT_TRUE(NULL != img0_->mem);
    EXPECT_TRUE(NULL != img0_->data);
    EXPECT_EQ(640, img0_->width);
    EXPECT_EQ(480, img0_->height);
    EXPECT_EQ(1, img0_->n_channels);
    nx_image_free(img0_);
}

TEST_F(NXImageTest, ImageNewRGBAFree) {
    img0_ = nx_image_new_rgba(640, 480);
    EXPECT_TRUE(NULL != img0_);
    EXPECT_TRUE(NULL != img0_->mem);
    EXPECT_TRUE(NULL != img0_->data);
    EXPECT_EQ(640, img0_->width);
    EXPECT_EQ(480, img0_->height);
    EXPECT_EQ(4, img0_->n_channels);
    nx_image_free(img0_);
}

TEST_F(NXImageTest, ImageGraySaveLoadAsIs) {
    img0_ = nx_image_new_gray(NX, NY);
    memcpy(img0_->data, TEST_IMAGE_DATA, sizeof(TEST_IMAGE_DATA));
    nx_image_xsave_pnm(img0_, TMP_PGM_IMAGE_FILENAME);
    img1_ = nx_image_alloc();
    nx_image_xload_pnm(img1_, TMP_PGM_IMAGE_FILENAME, NX_IMAGE_LOAD_AS_IS);
    for (int i = 0; i < N; ++i)
        EXPECT_EQ(TEST_IMAGE_DATA[i], img1_->data[i]);
    nx_image_free(img0_);
    nx_image_free(img1_);
}

TEST_F(NXImageTest, ImageRGBASaveLoadAsIs) {
    img0_ = nx_image_new_rgba(NX, NY);
    memcpy(img0_->data, TEST_IMAGE_COLOR_DATA, sizeof(TEST_IMAGE_COLOR_DATA));
    nx_image_xsave_pnm(img0_, TMP_PPM_IMAGE_FILENAME);
    img1_ = nx_image_alloc();
    nx_image_xload_pnm(img1_, TMP_PPM_IMAGE_FILENAME, NX_IMAGE_LOAD_AS_IS);
    for (int i = 0; i < 4*N; ++i)
        EXPECT_EQ(TEST_IMAGE_COLOR_DATA[i], img1_->data[i]);
    nx_image_free(img0_);
    nx_image_free(img1_);
}

TEST_F(NXImageTest, ImageGrayFilterSI) {
    img0_ = nx_image_new_gray(NX, NY);
    memcpy(img0_->data, TEST_IMAGE_DATA, sizeof(TEST_IMAGE_DATA));
    img1_ = nx_image_alloc();
    nx_image_smooth_si(img1_, img0_, TEST_SIGMA, TEST_SIGMA, NULL);
    nx_image_free(img0_);
    nx_image_free(img1_);
}

TEST_F(NXImageTest, ImageGrayFilterS) {
    img0_ = nx_image_new_gray(NX, NY);
    memcpy(img0_->data, TEST_IMAGE_DATA, sizeof(TEST_IMAGE_DATA));
    img1_ = nx_image_alloc();
    nx_image_smooth_s(img1_, img0_, TEST_SIGMA, TEST_SIGMA, NULL);
    nx_image_free(img0_);
    nx_image_free(img1_);
}




} // namespace
