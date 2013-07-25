/**
 * @file tests_image_pyr.cc
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
#include "virg/nexus/nx_image_pyr.h"

using std::pow;

namespace {

const int TEST_WIDTH0 = 97;
const int TEST_HEIGHT0 = 97;
const int TEST_N_LEVELS = 3;
const int TEST_OCTAVES = 3;
const int TEST_STEPS = 3;
const float TEST_SCALE_F = 1.2f;
const float TEST_SIGMA0 = 1.5f;

class NXImagePyrTest : public ::testing::Test {
protected:
        NXImagePyrTest() {
                pyr0_ = NULL;
        }

        virtual void SetUp() {}

        virtual void TearDown() {}

        struct NXImagePyr *pyr0_;
};

TEST_F(NXImagePyrTest, ImagePyrAllocFree) {
        pyr0_ = nx_image_pyr_alloc();
        EXPECT_TRUE(NULL != pyr0_);
        EXPECT_TRUE(NULL != pyr0_->work_img);
        EXPECT_EQ(0, pyr0_->n_levels);
        nx_image_pyr_free(pyr0_);
}

TEST_F(NXImagePyrTest, ImagePyrNewFastFree) {
        pyr0_ = nx_image_pyr_new_fast(TEST_WIDTH0, TEST_HEIGHT0, TEST_N_LEVELS);
        EXPECT_TRUE(NULL != pyr0_);
        EXPECT_TRUE(NULL != pyr0_->levels);
        EXPECT_TRUE(NULL != pyr0_->work_img);
        EXPECT_EQ(TEST_WIDTH0, pyr0_->info.fast.width0);
        EXPECT_EQ(TEST_HEIGHT0, pyr0_->info.fast.height0);
        EXPECT_EQ(TEST_N_LEVELS, pyr0_->n_levels);
        for (int i = 0; i < TEST_N_LEVELS; ++i) {
                EXPECT_TRUE(NULL != pyr0_->levels[i].img);
                EXPECT_EQ(TEST_WIDTH0 / (1 << i), pyr0_->levels[i].img->width);
                EXPECT_EQ(TEST_HEIGHT0 / (1 << i), pyr0_->levels[i].img->height);
                EXPECT_EQ(NX_IMAGE_GRAYSCALE, pyr0_->levels[i].img->type);
        }

        nx_image_pyr_free(pyr0_);
}

TEST_F(NXImagePyrTest, ImagePyrNewFineFree) {
        pyr0_ = nx_image_pyr_new_fine(TEST_WIDTH0, TEST_HEIGHT0, TEST_OCTAVES, TEST_STEPS, TEST_SIGMA0);
        EXPECT_TRUE(NULL != pyr0_);
        EXPECT_TRUE(NULL != pyr0_->levels);
        EXPECT_TRUE(NULL != pyr0_->work_img);
        EXPECT_EQ(TEST_WIDTH0, pyr0_->info.fine.width0);
        EXPECT_EQ(TEST_HEIGHT0, pyr0_->info.fine.height0);
        EXPECT_EQ(TEST_OCTAVES, pyr0_->info.fine.n_octaves);
        EXPECT_EQ(TEST_STEPS, pyr0_->info.fine.n_octave_steps);
        EXPECT_EQ(TEST_SIGMA0, pyr0_->info.fine.sigma0);
        EXPECT_EQ(TEST_OCTAVES*TEST_STEPS, pyr0_->n_levels);
        for (int i = 0; i < TEST_N_LEVELS; ++i) {
                EXPECT_TRUE(NULL != pyr0_->levels[i].img);
                EXPECT_EQ(TEST_WIDTH0 / (1 << (i/TEST_STEPS)), pyr0_->levels[i].img->width);
                EXPECT_EQ(TEST_HEIGHT0 / (1 << (i/TEST_STEPS)), pyr0_->levels[i].img->height);
                EXPECT_EQ(NX_IMAGE_GRAYSCALE, pyr0_->levels[i].img->type);
        }

        nx_image_pyr_free(pyr0_);
}

TEST_F(NXImagePyrTest, ImagePyrNewScaledFree) {
        pyr0_ = nx_image_pyr_new_scaled(TEST_WIDTH0, TEST_HEIGHT0, TEST_N_LEVELS, TEST_SCALE_F, TEST_SIGMA0);
        EXPECT_TRUE(NULL != pyr0_);
        EXPECT_TRUE(NULL != pyr0_->levels);
        EXPECT_TRUE(NULL != pyr0_->work_img);
        EXPECT_EQ(TEST_WIDTH0, pyr0_->info.scaled.width0);
        EXPECT_EQ(TEST_HEIGHT0, pyr0_->info.scaled.height0);
        EXPECT_EQ(TEST_SCALE_F, pyr0_->info.scaled.scale_factor);
        EXPECT_EQ(TEST_SIGMA0, pyr0_->info.scaled.sigma0);
        EXPECT_EQ(TEST_N_LEVELS, pyr0_->n_levels);
        for (int i = 0; i < TEST_N_LEVELS; ++i) {
                EXPECT_TRUE(NULL != pyr0_->levels[i].img);
                EXPECT_EQ(static_cast<int>(TEST_WIDTH0 / pow(TEST_SCALE_F, i)), pyr0_->levels[i].img->width);
                EXPECT_EQ(static_cast<int>(TEST_HEIGHT0 / pow(TEST_SCALE_F, i)), pyr0_->levels[i].img->height);
                EXPECT_EQ(NX_IMAGE_GRAYSCALE, pyr0_->levels[i].img->type);
        }

        nx_image_pyr_free(pyr0_);
}

/*
  TEST_F(NXImagePyrTest, ImageNewRGBAFree) {
  pyr0_ = nx_image_pyr_new_rgba(640, 480);
  EXPECT_TRUE(NULL != pyr0_);
  EXPECT_TRUE(NULL != pyr0_->mem);
  EXPECT_TRUE(NULL != pyr0_->data);
  EXPECT_EQ(640, pyr0_->width);
  EXPECT_EQ(480, pyr0_->height);
  EXPECT_EQ(4, pyr0_->n_channels);
  nx_image_pyr_free(pyr0_);
  }

  TEST_F(NXImagePyrTest, ImageGraySaveLoadAsIs) {
  pyr0_ = nx_image_pyr_new_gray(NX, NY);
  memcpy(pyr0_->data, TEST_IMAGE_PYR_DATA, sizeof(TEST_IMAGE_PYR_DATA));
  nx_image_pyr_xsave_pnm(pyr0_, TMP_PGM_IMAGE_PYR_FILENAME);
  img1_ = nx_image_pyr_alloc();
  nx_image_pyr_xload_pnm(img1_, TMP_PGM_IMAGE_PYR_FILENAME, NX_IMAGE_PYR_LOAD_AS_IS);
  for (int i = 0; i < N; ++i)
  EXPECT_EQ(TEST_IMAGE_PYR_DATA[i], img1_->data[i]);
  nx_image_pyr_free(pyr0_);
  nx_image_pyr_free(img1_);
  }

  TEST_F(NXImagePyrTest, ImageRGBASaveLoadAsIs) {
  pyr0_ = nx_image_pyr_new_rgba(NX, NY);
  memcpy(pyr0_->data, TEST_IMAGE_PYR_COLOR_DATA, sizeof(TEST_IMAGE_PYR_COLOR_DATA));
  nx_image_pyr_xsave_pnm(pyr0_, TMP_PPM_IMAGE_PYR_FILENAME);
  img1_ = nx_image_pyr_alloc();
  nx_image_pyr_xload_pnm(img1_, TMP_PPM_IMAGE_PYR_FILENAME, NX_IMAGE_PYR_LOAD_AS_IS);
  for (int i = 0; i < 4*N; ++i)
  EXPECT_EQ(TEST_IMAGE_PYR_COLOR_DATA[i], img1_->data[i]);
  nx_image_pyr_free(pyr0_);
  nx_image_pyr_free(img1_);
  }

  TEST_F(NXImagePyrTest, ImageGrayFilterSI) {
  pyr0_ = nx_image_pyr_new_gray(NX, NY);
  memcpy(pyr0_->data, TEST_IMAGE_PYR_DATA, sizeof(TEST_IMAGE_PYR_DATA));
  img1_ = nx_image_pyr_alloc();
  nx_image_pyr_smooth_si(img1_, pyr0_, TEST_SIGMA, TEST_SIGMA, NULL);
  nx_image_pyr_free(pyr0_);
  nx_image_pyr_free(img1_);
  }

  TEST_F(NXImagePyrTest, ImageGrayFilterS) {
  pyr0_ = nx_image_pyr_new_gray(NX, NY);
  memcpy(pyr0_->data, TEST_IMAGE_PYR_DATA, sizeof(TEST_IMAGE_PYR_DATA));
  img1_ = nx_image_pyr_alloc();
  nx_image_pyr_smooth_s(img1_, pyr0_, TEST_SIGMA, TEST_SIGMA, NULL);
  nx_image_pyr_free(pyr0_);
  nx_image_pyr_free(img1_);
  }

*/


} // namespace
