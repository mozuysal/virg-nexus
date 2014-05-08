/**
 * @file tests_brief_extractor.cc
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

#include "test_data.hh"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_image_io.h"
#include "virg/nexus/nx_image_pyr.h"
#include "virg/nexus/nx_image_pyr_builder.h"
#include "virg/nexus/nx_brief_extractor.h"
#include "virg/nexus/nx_uniform_sampler.h"

using std::pow;

namespace {

const int TEST_N_LEVELS = 5;
const int TEST_OCTAVES = 4;
const int TEST_STEPS = 3;
const float TEST_SIGMA0 = 0.9f;
const int TEST_N_OCTETS = 2;
const int TEST_RADIUS = 16;
const int N_COMPUTE_TESTS = 1000;

const int TEST_IMAGE_W = 4;
const int TEST_IMAGE_H = 4;
const uchar TEST_IMAGE_DATA[] = {
        15-2,  15+3,  15-4,  15+9,
        15+1,  15+0,  15+5, 15+10,
        15-8,  15+7,  15-6, 15+11,
       15-15, 15-14, 15-13, 15+12,
};

const int TEST_OFFSETS[TEST_N_OCTETS * 8 * 4] = {
        0, 0, -1,  0, // 1
        0, 0, -1, -1, // 2
        0, 0,  0, -1, // 3
        0, 0, +1, -1, // 4
        0, 0, +1,  0, // 5
        0, 0, +1, +1, // 6
        0, 0,  0, +1, // 7
        0, 0, -1, +1, // 8
        0, 0, +2, -1, // 9
        0, 0, +2,  0, //10
        0, 0, +2, +1, //11
        0, 0, +2, +2, //12
        0, 0, +1, +2, //13
        0, 0,  0, +2, //14
        0, 0, -1, +2, //15
        0, 0,  0,  0, //16
};

const uchar TEST_DESC_0 = 0x55; // 01010101
const uchar TEST_DESC_1 = 0x0E; // 00001110

class NXBriefExtractorTest : public ::testing::Test {
protected:
        NXBriefExtractorTest() {
                lena_ = NULL;
                pyr0_ = NULL;
                pyr1_ = NULL;
                be_ = NULL;
                desc_ = NULL;
        }

        virtual void SetUp() {
                lena_ = nx_image_alloc();
                nx_image_xload_pnm(lena_, TEST_DATA_LENA_PPM, NX_IMAGE_LOAD_GRAYSCALE);

                desc_ = NX_NEW_UC(TEST_N_OCTETS);
                builder_ = nx_image_pyr_builder_alloc();
        }

        virtual void TearDown() {
                nx_image_pyr_builder_free(builder_);
                nx_free(desc_);
                nx_image_free(lena_);
                nx_uniform_sampler_instance_free();
        }

        void SetUpPyramid0() {
                nx_image_pyr_builder_set_fast(builder_, TEST_N_LEVELS, TEST_SIGMA0);
                pyr0_ = nx_image_pyr_builder_build0(builder_, lena_);
        }

        void SetUpPyramid1() {
                nx_image_pyr_builder_set_fine(builder_, TEST_OCTAVES, TEST_STEPS, TEST_SIGMA0);
                pyr1_ = nx_image_pyr_builder_build0(builder_, lena_);
        }

        void TearDownPyramids() {
                nx_image_pyr_free(pyr0_);
                nx_image_pyr_free(pyr1_);
        }

        struct NXImage *lena_;
        struct NXImagePyr *pyr0_;
        struct NXImagePyr *pyr1_;
        struct NXImagePyrBuilder *builder_;

        uchar *desc_;

        struct NXBriefExtractor *be_;
};

TEST_F(NXBriefExtractorTest, BriefExtractorAllocFree) {
        be_ = nx_brief_extractor_alloc();
        EXPECT_TRUE(NULL != be_);
        EXPECT_TRUE(NULL == be_->offsets);
        EXPECT_EQ(0, be_->n_octets);
        nx_brief_extractor_free(be_);
}

TEST_F(NXBriefExtractorTest, BriefExtractorNewFree) {
        be_ = nx_brief_extractor_new(TEST_N_OCTETS, TEST_RADIUS);
        EXPECT_TRUE(NULL != be_);
        EXPECT_TRUE(NULL != be_->offsets);
        EXPECT_EQ(TEST_N_OCTETS, be_->n_octets);
        EXPECT_EQ(TEST_RADIUS, be_->radius);
        nx_brief_extractor_free(be_);
}

TEST_F(NXBriefExtractorTest, BriefExtractorCheckPtPyr) {
        be_ = nx_brief_extractor_new(TEST_N_OCTETS, TEST_RADIUS);
        SetUpPyramid0();
        int hw = lena_->width/2;
        int hh = lena_->height/2;
        EXPECT_TRUE(nx_brief_extractor_check_point_pyr(be_, pyr0_, hw, hh, 0));
        int level = pyr0_->n_levels - be_->pyr_level_offset - 1;
        float sf = pyr0_->levels[level].scale;
        EXPECT_TRUE(nx_brief_extractor_check_point_pyr(be_, pyr0_, hw/sf, hh/sf, level));
        sf = pyr0_->levels[level+1].scale;
        EXPECT_FALSE(nx_brief_extractor_check_point_pyr(be_, pyr0_, hw/sf, hh/sf, level+1));
        EXPECT_FALSE(nx_brief_extractor_check_point_pyr(be_, pyr0_, 0, 0, 0));
        EXPECT_FALSE(nx_brief_extractor_check_point_pyr(be_, pyr0_, 0, hh*2-1, 0));
        EXPECT_FALSE(nx_brief_extractor_check_point_pyr(be_, pyr0_, hw*2-1, 0, 0));
        EXPECT_FALSE(nx_brief_extractor_check_point_pyr(be_, pyr0_, hw*2-1, hh*2-1, 0));
        nx_brief_extractor_free(be_);
        TearDownPyramids();
}

TEST_F(NXBriefExtractorTest, BriefExtractorComputePyrFast) {
        be_ = nx_brief_extractor_new(TEST_N_OCTETS, TEST_RADIUS);
        SetUpPyramid0();

        int n = 0;
        for (int i = 0; i < N_COMPUTE_TESTS; ++i) {
                int level = pyr0_->n_levels * NX_UNIFORM_SAMPLE_S;
                int x = pyr0_->levels[level].img->width * NX_UNIFORM_SAMPLE_S;
                int y = pyr0_->levels[level].img->height * NX_UNIFORM_SAMPLE_S;

                if (nx_brief_extractor_check_point_pyr(be_, pyr0_, x, y, level)) {
                        n++;
                        nx_brief_extractor_compute_pyr(be_, pyr0_, x, y, level, desc_);
                }
        }
        EXPECT_LE(N_COMPUTE_TESTS / 3, n);

        nx_brief_extractor_free(be_);
        TearDownPyramids();
}

TEST_F(NXBriefExtractorTest, BriefExtractorComputePyrFine) {
        be_ = nx_brief_extractor_new(TEST_N_OCTETS, TEST_RADIUS);
        SetUpPyramid1();

        int n = 0;
        for (int i = 0; i < N_COMPUTE_TESTS; ++i) {
                int level = pyr1_->n_levels * NX_UNIFORM_SAMPLE_S;
                int x = pyr1_->levels[level].img->width * NX_UNIFORM_SAMPLE_S;
                int y = pyr1_->levels[level].img->height * NX_UNIFORM_SAMPLE_S;

                if (nx_brief_extractor_check_point_pyr(be_, pyr1_, x, y, level)) {
                        n++;
                        nx_brief_extractor_compute_pyr(be_, pyr1_, x, y, level, desc_);
                }
        }
        EXPECT_LE(N_COMPUTE_TESTS / 2, n);

        nx_brief_extractor_free(be_);
        TearDownPyramids();
}

TEST_F(NXBriefExtractorTest, BriefExtractorUpdateLimits) {
        be_ = nx_brief_extractor_new(TEST_N_OCTETS, TEST_RADIUS);
        memcpy(be_->offsets, TEST_OFFSETS, sizeof(TEST_OFFSETS));
        nx_brief_extractor_update_limits(be_);

        EXPECT_EQ(-1, be_->offset_limits[0]);
        EXPECT_EQ(+2, be_->offset_limits[1]);
        EXPECT_EQ(-1, be_->offset_limits[2]);
        EXPECT_EQ(+2, be_->offset_limits[3]);

        nx_brief_extractor_free(be_);
}

TEST_F(NXBriefExtractorTest, BriefExtractorComputePyrArtificial) {
        be_ = nx_brief_extractor_new(TEST_N_OCTETS, TEST_RADIUS);
        memcpy(be_->offsets, TEST_OFFSETS, sizeof(TEST_OFFSETS));
        nx_brief_extractor_update_limits(be_);

        struct NXImage *img = nx_image_alloc();
        nx_image_resize(img, TEST_IMAGE_W, TEST_IMAGE_H, 0, NX_IMAGE_GRAYSCALE);
        for (int i = 0; i < TEST_IMAGE_H; ++i)
                memcpy(img->data + i*img->row_stride,
                       TEST_IMAGE_DATA + i*TEST_IMAGE_W,
                       TEST_IMAGE_W * sizeof(uchar));
        nx_image_pyr_builder_set_fast(builder_, 1, 0);
        pyr0_ = nx_image_pyr_builder_build0(builder_, img);
        be_->pyr_level_offset = 0;

        EXPECT_TRUE(nx_brief_extractor_check_point_pyr(be_, pyr0_, 1, 1, 0));

        nx_brief_extractor_compute_pyr(be_, pyr0_, 1, 1, 0, desc_);
        EXPECT_EQ(TEST_DESC_0, desc_[0]);
        EXPECT_EQ(TEST_DESC_1, desc_[1]);

        nx_image_pyr_free(pyr0_);
        nx_image_free(img);
        nx_brief_extractor_free(be_);
}

} // namespace
