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

#include "test_data.hh"

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
                lena_ = NULL;
                pyr0_ = NULL;
        }

        virtual void SetUp() {
                lena_ = nx_image_alloc();
                nx_image_xload_pnm(lena_, TEST_DATA_LENA_PPM, NX_IMAGE_LOAD_GRAYSCALE);
        }

        virtual void TearDown() {
                nx_image_free(lena_);
        }

        struct NXImage *lena_;
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
        pyr0_ = nx_image_pyr_new_fast(TEST_WIDTH0, TEST_HEIGHT0, TEST_N_LEVELS, TEST_SIGMA0);
        EXPECT_TRUE(NULL != pyr0_);
        EXPECT_TRUE(NULL != pyr0_->levels);
        EXPECT_TRUE(NULL != pyr0_->work_img);
        EXPECT_EQ(TEST_WIDTH0, pyr0_->info.fast.width0);
        EXPECT_EQ(TEST_HEIGHT0, pyr0_->info.fast.height0);
        EXPECT_EQ(TEST_N_LEVELS, pyr0_->n_levels);
        EXPECT_EQ(TEST_SIGMA0, pyr0_->info.fast.sigma0);
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

TEST_F(NXImagePyrTest, ImagePyrComputeFastLena) {
        pyr0_ = nx_image_pyr_new_fast(lena_->width, lena_->height, TEST_N_LEVELS, TEST_SIGMA0);
        nx_image_pyr_compute(pyr0_, lena_);

        EXPECT_TRUE(NULL != pyr0_);
        EXPECT_TRUE(NULL != pyr0_->levels);
        EXPECT_TRUE(NULL != pyr0_->work_img);
        EXPECT_EQ(lena_->width, pyr0_->info.fast.width0);
        EXPECT_EQ(lena_->height, pyr0_->info.fast.height0);
        EXPECT_EQ(TEST_N_LEVELS, pyr0_->n_levels);
        for (int i = 0; i < TEST_N_LEVELS; ++i) {
                EXPECT_TRUE(NULL != pyr0_->levels[i].img);
                EXPECT_EQ(lena_->width / (1 << i), pyr0_->levels[i].img->width);
                EXPECT_EQ(lena_->height / (1 << i), pyr0_->levels[i].img->height);
                EXPECT_EQ(NX_IMAGE_GRAYSCALE, pyr0_->levels[i].img->type);
        }

        nx_image_pyr_free(pyr0_);
}

TEST_F(NXImagePyrTest, ImagePyrComputeFineLena) {
        pyr0_ = nx_image_pyr_new_fine(lena_->width, lena_->height, TEST_OCTAVES, TEST_STEPS, TEST_SIGMA0);
        nx_image_pyr_compute(pyr0_, lena_);

        EXPECT_TRUE(NULL != pyr0_);
        EXPECT_TRUE(NULL != pyr0_->levels);
        EXPECT_TRUE(NULL != pyr0_->work_img);
        EXPECT_EQ(lena_->width, pyr0_->info.fine.width0);
        EXPECT_EQ(lena_->height, pyr0_->info.fine.height0);
        EXPECT_EQ(TEST_OCTAVES, pyr0_->info.fine.n_octaves);
        EXPECT_EQ(TEST_STEPS, pyr0_->info.fine.n_octave_steps);
        EXPECT_EQ(TEST_SIGMA0, pyr0_->info.fine.sigma0);
        EXPECT_EQ(TEST_OCTAVES*TEST_STEPS, pyr0_->n_levels);
        for (int i = 0; i < TEST_N_LEVELS; ++i) {
                EXPECT_TRUE(NULL != pyr0_->levels[i].img);
                EXPECT_EQ(lena_->width / (1 << (i/TEST_STEPS)), pyr0_->levels[i].img->width);
                EXPECT_EQ(lena_->height / (1 << (i/TEST_STEPS)), pyr0_->levels[i].img->height);
                EXPECT_EQ(NX_IMAGE_GRAYSCALE, pyr0_->levels[i].img->type);
        }

        nx_image_pyr_free(pyr0_);
}

TEST_F(NXImagePyrTest, ImagePyrComputeScaledFree) {
        pyr0_ = nx_image_pyr_new_scaled(lena_->width, lena_->height, TEST_N_LEVELS, TEST_SCALE_F, TEST_SIGMA0);
        nx_image_pyr_compute(pyr0_, lena_);

        EXPECT_TRUE(NULL != pyr0_);
        EXPECT_TRUE(NULL != pyr0_->levels);
        EXPECT_TRUE(NULL != pyr0_->work_img);
        EXPECT_EQ(lena_->width, pyr0_->info.scaled.width0);
        EXPECT_EQ(lena_->height, pyr0_->info.scaled.height0);
        EXPECT_EQ(TEST_SCALE_F, pyr0_->info.scaled.scale_factor);
        EXPECT_EQ(TEST_SIGMA0, pyr0_->info.scaled.sigma0);
        EXPECT_EQ(TEST_N_LEVELS, pyr0_->n_levels);
        for (int i = 0; i < TEST_N_LEVELS; ++i) {
                EXPECT_TRUE(NULL != pyr0_->levels[i].img);
                EXPECT_EQ(static_cast<int>(lena_->width / pow(TEST_SCALE_F, i)), pyr0_->levels[i].img->width);
                EXPECT_EQ(static_cast<int>(lena_->height / pow(TEST_SCALE_F, i)), pyr0_->levels[i].img->height);
                EXPECT_EQ(NX_IMAGE_GRAYSCALE, pyr0_->levels[i].img->type);
        }

        nx_image_pyr_free(pyr0_);
}

} // namespace
