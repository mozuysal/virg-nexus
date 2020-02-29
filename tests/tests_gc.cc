/**
 * @file tests_gc.cc
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

#include "virg/nexus/nx_gc.h"
#include "virg/nexus/nx_image_io.h"

using namespace std;

namespace {

static const char TMP_PDF_FILENAME[] = "/tmp/nx_gc.pdf";
static const char TMP_PPM_FILENAME[] = "/tmp/nx_gc.ppm";
static const char TMP_SVG_FILENAME[] = "/tmp/nx_gc.svg";

class NXGCTest : public ::testing::Test {
protected:
        NXGCTest()
                {
                        gc_ = NULL;
                        img_ = NULL;
                }

        virtual void SetUp()
                {
                }

        virtual void TearDown()
                {
                }

        struct NXGC *gc_;
        struct NXImage *img_;
};

TEST_F(NXGCTest, GCNewPDFFree) {
        gc_ = nx_gc_new_pdf(TMP_PDF_FILENAME, NX_GC_A4_WIDTH, NX_GC_A4_HEIGHT);
        EXPECT_TRUE(NULL != gc_);
        nx_gc_free(gc_);
}

TEST_F(NXGCTest, GCNewSVGFree) {
        gc_ = nx_gc_new_svg(TMP_SVG_FILENAME, NX_GC_A4_WIDTH, NX_GC_A4_HEIGHT);
        EXPECT_TRUE(NULL != gc_);
        nx_gc_free(gc_);
}

TEST_F(NXGCTest, GCNewFree) {
        img_ = nx_image_new_rgba_uc(640, 480);
        EXPECT_TRUE(NULL != img_);
        gc_ = nx_gc_new(img_);
        EXPECT_TRUE(NULL != gc_);
        nx_gc_free(gc_);
        nx_image_free(img_);
}

TEST_F(NXGCTest, GCBlueSquareOnWhitePDF) {
        gc_ = nx_gc_new_pdf(TMP_PDF_FILENAME, NX_GC_A4_WIDTH, NX_GC_A4_HEIGHT);
        EXPECT_TRUE(NULL != gc_);
        nx_gc_set_color(gc_, 0.0, 0.0, 1.0, 1.0);
        double x = NX_GC_A4_WIDTH / 4.0;
        double y = NX_GC_A4_HEIGHT / 4.0;
        double w = NX_GC_A4_WIDTH / 2.0;
        double h = NX_GC_A4_HEIGHT / 2.0;
        nx_gc_rectangle(gc_, x, y, w, h);
        nx_gc_fill(gc_);
        nx_gc_free(gc_);
}

TEST_F(NXGCTest, GCBlueSquareOnWhiteSVG) {
        gc_ = nx_gc_new_svg(TMP_SVG_FILENAME, NX_GC_A4_WIDTH, NX_GC_A4_HEIGHT);
        EXPECT_TRUE(NULL != gc_);
        nx_gc_set_color(gc_, 0.0, 0.0, 1.0, 1.0);
        double x = NX_GC_A4_WIDTH / 4.0;
        double y = NX_GC_A4_HEIGHT / 4.0;
        double w = NX_GC_A4_WIDTH / 2.0;
        double h = NX_GC_A4_HEIGHT / 2.0;
        nx_gc_rectangle(gc_, x, y, w, h);
        nx_gc_fill(gc_);
        nx_gc_free(gc_);
}

TEST_F(NXGCTest, GCBlueSquareOnWhite) {
        img_ = nx_image_new_rgba_uc(640, 480);
        EXPECT_TRUE(NULL != img_);
        gc_ = nx_gc_new(img_);
        EXPECT_TRUE(NULL != gc_);
        nx_gc_set_color(gc_, 1.0, 1.0, 1.0, 1.0);
        nx_gc_clear(gc_);
        nx_gc_set_color(gc_, 0.0, 0.0, 1.0, 1.0);
        double x = 640 / 4.0;
        double y = 480 / 4.0;
        double w = 640 / 2.0;
        double h = 480 / 2.0;
        nx_gc_rectangle(gc_, x, y, w, h);
        nx_gc_fill(gc_);
        nx_gc_flush(gc_);
        nx_image_xsave_pnm(img_, TMP_PPM_FILENAME);
        nx_gc_free(gc_);
        nx_image_free(img_);
}

} // namespace
