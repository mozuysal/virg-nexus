/**
 * @file tests_fast_detector.cc
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
#include "virg/nexus/nx_fast_detector.h"

using std::pow;

namespace {

const int TEST_N_LEVELS = 3;
const float TEST_SIGMA0 = 0.0f;
const int TEST_MAX_N_KEYS = 1000;

class NXFastDetectorTest : public ::testing::Test {
protected:
        NXFastDetectorTest() {
                lena_ = NULL;
                pyr_ = NULL;
        }

        virtual void SetUp() {
                lena_ = nx_image_alloc();
                nx_image_xload_pnm(lena_, TEST_DATA_LENA_PPM, NX_IMAGE_LOAD_GRAYSCALE);

                pyr_ = nx_image_pyr_new_fast(lena_->width, lena_->height, TEST_N_LEVELS, TEST_SIGMA0);
                nx_image_pyr_compute(pyr_, lena_);
        }

        virtual void TearDown() {
                nx_image_pyr_free(pyr_);
                nx_image_free(lena_);
        }

        struct NXImage *lena_;
        struct NXImagePyr *pyr_;

        struct NXFastDetector *det_;
};

TEST_F(NXFastDetectorTest, FastDetectorAllocFree) {
        det_ = nx_fast_detector_alloc();
        EXPECT_TRUE(NULL != det_);
        EXPECT_TRUE(NULL != det_->mem);
        EXPECT_EQ(0, det_->n_keys);
        EXPECT_EQ(NULL, det_->keys);
        EXPECT_EQ(0, det_->n_work);
        EXPECT_EQ(NULL, det_->keys_work);
        nx_fast_detector_free(det_);
}

TEST_F(NXFastDetectorTest, FastDetectorNewFastFree) {
        det_ = nx_fast_detector_new(TEST_MAX_N_KEYS, 0);
        EXPECT_TRUE(NULL != det_);
        EXPECT_TRUE(NULL != det_->keys);
        EXPECT_TRUE(NULL != det_->keys_work);
        EXPECT_TRUE(NULL != det_->mem);
        EXPECT_EQ(TEST_MAX_N_KEYS, det_->max_n_keys);
        EXPECT_LE(TEST_MAX_N_KEYS, det_->n_work);
        nx_fast_detector_free(det_);
}

TEST_F(NXFastDetectorTest, FastDetectorDetect) {
        det_ = nx_fast_detector_new(TEST_MAX_N_KEYS, 0);
        nx_fast_detector_detect(det_, pyr_->levels[0].img);
        EXPECT_GE(TEST_MAX_N_KEYS, det_->n_keys);
        EXPECT_LT(0, det_->n_keys);
        nx_fast_detector_free(det_);
}

TEST_F(NXFastDetectorTest, FastDetectorDetectPyr) {
        det_ = nx_fast_detector_new(TEST_MAX_N_KEYS, 0);
        nx_fast_detector_detect_pyr(det_, pyr_, -1);
        EXPECT_GE(TEST_MAX_N_KEYS, det_->n_keys);
        EXPECT_LT(0, det_->n_keys);
        nx_fast_detector_free(det_);
}

} // namespace
