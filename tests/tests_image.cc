/**
 * @file tests_image.cc
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Mustafa Ozuysal. All rights reserved.
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
#include "virg/nexus/nx_image_io.h"

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
static const uchar TEST_IMAGE_JPEG_COLOR_DATA[N*4] = { 29, 0, 131, 255, 31, 0, 133, 255, 50, 0, 110, 255, 52, 0, 112, 255,
                                                       36, 2, 138, 255, 38, 4, 140, 255, 57, 3, 117, 255, 59, 5, 119, 255,
                                                       110, 0, 50, 255, 112, 0, 52, 255, 131, 0, 31, 255, 133, 0, 33, 255,
                                                       117, 2, 57, 255, 119, 4, 59, 255, 138, 2, 38, 255, 140, 4, 40, 255 };


static const char TMP_PGM_IMAGE_FILENAME[] = "/tmp/nx_tmp_image.pgm";
static const char TMP_PPM_IMAGE_FILENAME[] = "/tmp/nx_tmp_image.ppm";
static const char TMP_JPEG_IMAGE_FILENAME[] = "/tmp/nx_tmp_image.jpg";

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

TEST_F(NXImageTest, ImageGraySaveLoadAsIsJPEG) {
        img0_ = nx_image_new_gray(NX, NY);
        memcpy(img0_->data, TEST_IMAGE_DATA, sizeof(TEST_IMAGE_DATA));
        nx_image_xsave_jpeg(img0_, TMP_JPEG_IMAGE_FILENAME);
        img1_ = nx_image_alloc();
        nx_image_xload_jpeg(img1_, TMP_JPEG_IMAGE_FILENAME, NX_IMAGE_LOAD_AS_IS);
        for (int i = 0; i < N; ++i)
                EXPECT_EQ(TEST_IMAGE_DATA[i], img1_->data[i]);
        nx_image_free(img0_);
        nx_image_free(img1_);
}

TEST_F(NXImageTest, ImageRGBASaveLoadAsIsJPEG) {
        img0_ = nx_image_new_rgba(NX, NY);
        memcpy(img0_->data, TEST_IMAGE_COLOR_DATA, sizeof(TEST_IMAGE_COLOR_DATA));
        nx_image_xsave_jpeg(img0_, TMP_JPEG_IMAGE_FILENAME);
        img1_ = nx_image_alloc();
        nx_image_xload_jpeg(img1_, TMP_JPEG_IMAGE_FILENAME, NX_IMAGE_LOAD_AS_IS);
        for (int i = 0; i < 4*N; ++i)
                EXPECT_EQ(TEST_IMAGE_JPEG_COLOR_DATA[i], img1_->data[i]);
        nx_image_free(img0_);
        nx_image_free(img1_);
}

TEST_F(NXImageTest, ImageFileFormatFromFilename) {
        EXPECT_EQ(NX_IMAGE_FILE_FORMAT_PNM_BINARY, nx_image_file_format_from_filename("sad/sdf/avdf.pgm"));
        EXPECT_EQ(NX_IMAGE_FILE_FORMAT_PNM_BINARY, nx_image_file_format_from_filename("sad/sdf/avdf.ppm"));
        EXPECT_EQ(NX_IMAGE_FILE_FORMAT_PNG, nx_image_file_format_from_filename("sad/sdf/avdf.png"));
        EXPECT_EQ(NX_IMAGE_FILE_FORMAT_JPEG, nx_image_file_format_from_filename("sad/sdf/avdf.jpeg"));
        EXPECT_EQ(NX_IMAGE_FILE_FORMAT_JPEG, nx_image_file_format_from_filename("sad/sdf/avdf.jpg"));
        EXPECT_EQ(NX_IMAGE_FILE_FORMAT_JPEG, nx_image_file_format_from_filename("sad/sdf/avdf.JPEG"));
        EXPECT_EQ(NX_IMAGE_FILE_FORMAT_JPEG, nx_image_file_format_from_filename("sad/sdf/avdf.JPG"));
}

TEST_F(NXImageTest, ImageFileFormatFromHeader) {
        EXPECT_EQ(NX_IMAGE_FILE_FORMAT_PNM_BINARY, nx_image_file_format_from_header(TMP_PPM_IMAGE_FILENAME));
        EXPECT_EQ(NX_IMAGE_FILE_FORMAT_PNM_BINARY, nx_image_file_format_from_header(TMP_PGM_IMAGE_FILENAME));
        EXPECT_EQ(NX_IMAGE_FILE_FORMAT_JPEG, nx_image_file_format_from_header(TMP_JPEG_IMAGE_FILENAME));
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
