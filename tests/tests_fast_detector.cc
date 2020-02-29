/**
 * @file tests_fast_detector.cc
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
 * You should have received a copy of the GNU General Public License along with
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
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

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_image_io.h"
#include "virg/nexus/nx_image_pyr.h"
#include "virg/nexus/nx_image_pyr_builder.h"
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

                builder_ = nx_image_pyr_builder_new_fast(TEST_N_LEVELS, TEST_SIGMA0);
                pyr_ = nx_image_pyr_builder_build0(builder_, lena_);

                keys_ = NX_NEW(TEST_MAX_N_KEYS, struct NXKeypoint);
                n_keys_ = 0;
        }

        virtual void TearDown() {
                nx_image_pyr_free(pyr_);
                nx_image_pyr_builder_free(builder_);
                nx_image_free(lena_);
                nx_free(keys_);
        }

        struct NXImage *lena_;
        struct NXImagePyr *pyr_;
        struct NXImagePyrBuilder *builder_;

        struct NXFastDetector *det_;
        struct NXKeypoint *keys_;
        int n_keys_;
};

TEST_F(NXFastDetectorTest, FastDetectorAllocFree) {
        det_ = nx_fast_detector_alloc();
        EXPECT_TRUE(NULL != det_);
        EXPECT_TRUE(NULL != det_->keys_work);
        nx_fast_detector_free(det_);
}

TEST_F(NXFastDetectorTest, FastDetectorDetect) {
        det_ = nx_fast_detector_alloc();
        n_keys_ = nx_fast_detector_detect(det_, TEST_MAX_N_KEYS, keys_, pyr_->levels[0].img);
        EXPECT_GE(TEST_MAX_N_KEYS, n_keys_);
        EXPECT_LT(0, n_keys_);
        nx_fast_detector_free(det_);
}

TEST_F(NXFastDetectorTest, FastDetectorDetectPyr) {
        det_ = nx_fast_detector_alloc();
        n_keys_ = nx_fast_detector_detect_pyr(det_, TEST_MAX_N_KEYS, keys_, pyr_, -1);
        EXPECT_GE(TEST_MAX_N_KEYS, n_keys_);
        EXPECT_LT(0, n_keys_);
        nx_fast_detector_free(det_);
}

} // namespace
