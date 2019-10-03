/**
 * @file tests_image_pyr.cc
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

#include "test_data.hh"

#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_image_io.h"
#include "virg/nexus/nx_image_pyr.h"
#include "virg/nexus/nx_image_pyr_builder.h"

using std::pow;

extern bool IS_VALGRIND_RUN;

namespace {

// const int TEST_WIDTH0 = 97;
// const int TEST_HEIGHT0 = 97;
// const int TEST_N_LEVELS = 3;

int TEST_WIDTH0 = 921;
int TEST_HEIGHT0 = 614;
int TEST_N_LEVELS = 5;
int TEST_OCTAVES = 3;
int TEST_STEPS = 3;
float TEST_SCALE_F = 1.2f;
const float TEST_SIGMA0 = 1.5f;

class NXImagePyrTest : public ::testing::Test {
protected:
        NXImagePyrTest() {
                img_ = NULL;
                lena_ = NULL;
                pyr0_ = NULL;

                if (IS_VALGRIND_RUN) {
                        TEST_N_LEVELS = 2;
                        TEST_WIDTH0 = 321;
                        TEST_HEIGHT0 = 214;
                        TEST_OCTAVES = 2;
                        TEST_STEPS = 2;
                        TEST_SCALE_F = 1.8f;
                }
        }

        virtual void SetUp() {
                img_ = nx_image_new_gray_uc(TEST_WIDTH0, TEST_HEIGHT0);
                nx_image_set_zero(img_);
                lena_ = nx_image_alloc();
                nx_image_xload_pnm(lena_, TEST_DATA_LENA_PPM, NX_IMAGE_LOAD_GRAYSCALE);
        }

        virtual void TearDown() {
                nx_image_free(img_);
                nx_image_free(lena_);
        }

        struct NXImage *img_;
        struct NXImage *lena_;
        struct NXImagePyr *pyr0_;
        struct NXImagePyrBuilder *builder0_;
};

TEST_F(NXImagePyrTest, ImagePyrAllocFree) {
        pyr0_ = nx_image_pyr_alloc();
        EXPECT_TRUE(NULL != pyr0_);
        EXPECT_EQ(0, pyr0_->n_levels);
        nx_image_pyr_free(pyr0_);
}

TEST_F(NXImagePyrTest, ImagePyrBuilderAllocFree) {
        builder0_ = nx_image_pyr_builder_alloc();
        EXPECT_TRUE(NULL != builder0_);
        EXPECT_TRUE(NULL != builder0_->work_img);
        nx_image_pyr_builder_free(builder0_);
}

TEST_F(NXImagePyrTest, ImagePyrNewFastFree) {
        builder0_ = nx_image_pyr_builder_new_fast(TEST_N_LEVELS, TEST_SIGMA0);
        pyr0_ = nx_image_pyr_alloc();
        nx_image_pyr_builder_init_levels(builder0_, pyr0_, TEST_WIDTH0, TEST_HEIGHT0);
        EXPECT_TRUE(NULL != pyr0_);
        EXPECT_TRUE(NULL != pyr0_->levels);
        EXPECT_EQ(TEST_N_LEVELS, pyr0_->n_levels);
        for (int i = 0; i < TEST_N_LEVELS; ++i) {
                EXPECT_TRUE(NULL != pyr0_->levels[i].img);
                EXPECT_EQ(TEST_WIDTH0 / (1 << i), pyr0_->levels[i].img->width);
                EXPECT_EQ(TEST_HEIGHT0 / (1 << i), pyr0_->levels[i].img->height);
                EXPECT_EQ(NX_IMAGE_GRAYSCALE, pyr0_->levels[i].img->type);
        }

        nx_image_pyr_free(pyr0_);
        nx_image_pyr_builder_free(builder0_);
}

TEST_F(NXImagePyrTest, ImagePyrNewFineFree) {
        builder0_ = nx_image_pyr_builder_new_fine(TEST_OCTAVES, TEST_STEPS, TEST_SIGMA0);
        pyr0_ = nx_image_pyr_alloc();
        nx_image_pyr_builder_init_levels(builder0_, pyr0_, TEST_WIDTH0, TEST_HEIGHT0);
        EXPECT_TRUE(NULL != pyr0_);
        EXPECT_TRUE(NULL != pyr0_->levels);
        EXPECT_EQ(TEST_OCTAVES*TEST_STEPS, pyr0_->n_levels);
        for (int i = 0; i < TEST_N_LEVELS; ++i) {
                EXPECT_TRUE(NULL != pyr0_->levels[i].img);
                EXPECT_EQ(TEST_WIDTH0 / (1 << (i/TEST_STEPS)), pyr0_->levels[i].img->width);
                EXPECT_EQ(TEST_HEIGHT0 / (1 << (i/TEST_STEPS)), pyr0_->levels[i].img->height);
                EXPECT_EQ(NX_IMAGE_GRAYSCALE, pyr0_->levels[i].img->type);
        }

        nx_image_pyr_free(pyr0_);
        nx_image_pyr_builder_free(builder0_);
}

TEST_F(NXImagePyrTest, ImagePyrNewScaledFree) {
        builder0_ = nx_image_pyr_builder_new_scaled(TEST_N_LEVELS, TEST_SCALE_F, TEST_SIGMA0);
        pyr0_ = nx_image_pyr_alloc();
        nx_image_pyr_builder_init_levels(builder0_, pyr0_, TEST_WIDTH0, TEST_HEIGHT0);
        EXPECT_TRUE(NULL != pyr0_);
        EXPECT_TRUE(NULL != pyr0_->levels);
        EXPECT_EQ(TEST_N_LEVELS, pyr0_->n_levels);
        for (int i = 0; i < TEST_N_LEVELS; ++i) {
                EXPECT_TRUE(NULL != pyr0_->levels[i].img);
                EXPECT_EQ(static_cast<int>(TEST_WIDTH0 / pow(TEST_SCALE_F, i)), pyr0_->levels[i].img->width);
                EXPECT_EQ(static_cast<int>(TEST_HEIGHT0 / pow(TEST_SCALE_F, i)), pyr0_->levels[i].img->height);
                EXPECT_EQ(NX_IMAGE_GRAYSCALE, pyr0_->levels[i].img->type);
        }

        nx_image_pyr_free(pyr0_);
        nx_image_pyr_builder_free(builder0_);
}


TEST_F(NXImagePyrTest, ImagePyrComputeFast) {
        builder0_ = nx_image_pyr_builder_new_fast(TEST_N_LEVELS, TEST_SIGMA0);
        pyr0_ = nx_image_pyr_builder_build0(builder0_, img_);

        EXPECT_TRUE(NULL != pyr0_);
        EXPECT_TRUE(NULL != pyr0_->levels);
        EXPECT_EQ(TEST_N_LEVELS, pyr0_->n_levels);
        for (int i = 0; i < TEST_N_LEVELS; ++i) {
                EXPECT_TRUE(NULL != pyr0_->levels[i].img);
                EXPECT_EQ(img_->width / (1 << i), pyr0_->levels[i].img->width);
                EXPECT_EQ(img_->height / (1 << i), pyr0_->levels[i].img->height);
                EXPECT_EQ(NX_IMAGE_GRAYSCALE, pyr0_->levels[i].img->type);
        }

        nx_image_pyr_free(pyr0_);
        nx_image_pyr_builder_free(builder0_);
}

TEST_F(NXImagePyrTest, ImagePyrComputeFastLena) {
        builder0_ = nx_image_pyr_builder_new_fast(TEST_N_LEVELS, TEST_SIGMA0);
        pyr0_ = nx_image_pyr_builder_build0(builder0_, lena_);

        EXPECT_TRUE(NULL != pyr0_);
        EXPECT_TRUE(NULL != pyr0_->levels);
        EXPECT_EQ(TEST_N_LEVELS, pyr0_->n_levels);
        for (int i = 0; i < TEST_N_LEVELS; ++i) {
                EXPECT_TRUE(NULL != pyr0_->levels[i].img);
                EXPECT_EQ(lena_->width / (1 << i), pyr0_->levels[i].img->width);
                EXPECT_EQ(lena_->height / (1 << i), pyr0_->levels[i].img->height);
                EXPECT_EQ(NX_IMAGE_GRAYSCALE, pyr0_->levels[i].img->type);
        }

        nx_image_pyr_free(pyr0_);
        nx_image_pyr_builder_free(builder0_);
}

TEST_F(NXImagePyrTest, ImagePyrComputeFineLena) {
        builder0_ = nx_image_pyr_builder_new_fine(TEST_OCTAVES, TEST_STEPS, TEST_SIGMA0);
        pyr0_ = nx_image_pyr_builder_build0(builder0_, lena_);

        EXPECT_TRUE(NULL != pyr0_);
        EXPECT_TRUE(NULL != pyr0_->levels);
        EXPECT_EQ(TEST_OCTAVES*TEST_STEPS, pyr0_->n_levels);
        for (int i = 0; i < TEST_N_LEVELS; ++i) {
                EXPECT_TRUE(NULL != pyr0_->levels[i].img);
                EXPECT_EQ(lena_->width / (1 << (i/TEST_STEPS)), pyr0_->levels[i].img->width);
                EXPECT_EQ(lena_->height / (1 << (i/TEST_STEPS)), pyr0_->levels[i].img->height);
                EXPECT_EQ(NX_IMAGE_GRAYSCALE, pyr0_->levels[i].img->type);
        }

        nx_image_pyr_free(pyr0_);
        nx_image_pyr_builder_free(builder0_);
}

TEST_F(NXImagePyrTest, ImagePyrComputeScaledFree) {
        builder0_ = nx_image_pyr_builder_new_scaled(TEST_N_LEVELS, TEST_SCALE_F, TEST_SIGMA0);
        pyr0_ = nx_image_pyr_builder_build0(builder0_, lena_);

        EXPECT_TRUE(NULL != pyr0_);
        EXPECT_TRUE(NULL != pyr0_->levels);
        EXPECT_EQ(TEST_N_LEVELS, pyr0_->n_levels);
        for (int i = 0; i < TEST_N_LEVELS; ++i) {
                EXPECT_TRUE(NULL != pyr0_->levels[i].img);
                EXPECT_EQ(static_cast<int>(lena_->width / pow(TEST_SCALE_F, i)), pyr0_->levels[i].img->width);
                EXPECT_EQ(static_cast<int>(lena_->height / pow(TEST_SCALE_F, i)), pyr0_->levels[i].img->height);
                EXPECT_EQ(NX_IMAGE_GRAYSCALE, pyr0_->levels[i].img->type);
        }

        nx_image_pyr_free(pyr0_);
        nx_image_pyr_builder_free(builder0_);
}

} // namespace
