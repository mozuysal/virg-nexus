/**
 * @file tests_data_frame.cc
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2015 Mustafa Ozuysal. All rights reserved.
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

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_data_frame.h"

using namespace std;

namespace {

class NXDataFrameTest : public ::testing::Test {
protected:
        NXDataFrameTest() {
        }

        virtual void SetUp() {
        }

        virtual void TearDown() {
        }
};

TEST_F(NXDataFrameTest, alloc) {
        struct NXDataFrame *df = nx_data_frame_alloc();
        EXPECT_TRUE(df != NULL);
        nx_data_frame_free(df);
}

} // namespace
