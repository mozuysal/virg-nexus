/**
 * @file tests_string.cc
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
#include "virg/nexus/nx_string.h"

using namespace std;

namespace {

const char *TS1 = "TEST";
const char *TS2 = "ABCDEFG";
const char *TFS = "filename_%03d.%s";

class NXStringTest : public ::testing::Test {
protected:
        NXStringTest() {
        }

        virtual void SetUp() {
                p = nx_strdup(TS1);
                q = NULL;
        }

        virtual void TearDown() {
                nx_free(p);
                nx_free(q);
        }

        char *p;
        char *q;

};

TEST_F(NXStringTest, strdup) {
        char *s = nx_strdup(TS1);
        EXPECT_TRUE(s != NULL);
        EXPECT_STREQ(TS1, s);
        nx_free(s);
}

TEST_F(NXStringTest, strredup) {
        char *s = NULL;
        nx_strredup(&s, p);
        EXPECT_TRUE(s != NULL);
        EXPECT_STREQ(TS1, s);
        nx_free(s);
}

TEST_F(NXStringTest, strredup2) {
        char *s = nx_strdup(TS2);
        nx_strredup(&s, p);
        EXPECT_TRUE(s != NULL);
        EXPECT_STREQ(TS1, s);
        nx_free(s);
}

TEST_F(NXStringTest, strredup_null) {
        char *s = nx_strdup(TS2);
        nx_strredup(&s, q);
        EXPECT_TRUE(s == NULL);
        nx_free(s);
}

TEST_F(NXStringTest, strdup_redup) {
        char *s = nx_strdup(TS2);

        nx_strredup(&q, NULL);
        EXPECT_TRUE(q == NULL);
        nx_strredup(&q, s);
        EXPECT_STREQ(TS2, q);
        nx_strredup(&q, p);
        EXPECT_STREQ(TS1, q);
        nx_strredup(&q, NULL);
        EXPECT_TRUE(q == NULL);
        nx_strredup(&q, TS2);
        EXPECT_STREQ(TS2, q);

        nx_free(s);
}

TEST_F(NXStringTest, fstr) {
        char *fs = nx_fstr(TFS, 2, "png");
        EXPECT_STREQ("filename_002.png", fs);
        nx_free(fs);
}

} // namespace
