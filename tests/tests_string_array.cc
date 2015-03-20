/**
 * @file tests_string_array.cc
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
#include "virg/nexus/nx_string_array.h"

using namespace std;

namespace {

class NXStringArrayTest : public ::testing::Test {
protected:
        NXStringArrayTest() {
        }

        virtual void SetUp() {
                sa = nx_string_array_alloc();
        }

        virtual void TearDown() {
                nx_string_array_free(sa);
        }

        struct NXStringArray *sa;
};

TEST_F(NXStringArrayTest, alloc) {
        EXPECT_TRUE(sa != NULL);
        EXPECT_EQ(0, nx_string_array_size(sa));
}

TEST_F(NXStringArrayTest, resize) {
        nx_string_array_resize(sa, 2);
        EXPECT_EQ(2, nx_string_array_size(sa));
        EXPECT_TRUE(nx_string_array_get(sa, 0) == NULL);
        EXPECT_TRUE(nx_string_array_get(sa, 1) == NULL);
}

TEST_F(NXStringArrayTest, set) {
        nx_string_array_resize(sa, 2);
        nx_string_array_set(sa, 0, "abcd");
        EXPECT_STREQ("abcd", nx_string_array_get(sa, 0));
}

TEST_F(NXStringArrayTest, setf) {
        nx_string_array_resize(sa, 2);
        nx_string_array_setf(sa, 1, "abcd%03d", 3);
        EXPECT_STREQ("abcd003", nx_string_array_get(sa, 1));
}

TEST_F(NXStringArrayTest, clear) {
        nx_string_array_resize(sa, 8);
        nx_string_array_clear(sa);
        EXPECT_EQ(0, nx_string_array_size(sa));
}

TEST_F(NXStringArrayTest, downsize) {
        nx_string_array_resize(sa, 8);
        nx_string_array_set(sa, 0, "abcd");
        nx_string_array_set(sa, 1, "abcd");
        nx_string_array_resize(sa, 1);
        EXPECT_STREQ("abcd", nx_string_array_get(sa, 0));
        EXPECT_EQ(1, nx_string_array_size(sa));
}

TEST_F(NXStringArrayTest, append) {
        for (int i = 0; i < 10; ++i)
                nx_string_array_append(sa, "elem");

        EXPECT_EQ(10, nx_string_array_size(sa));
        for (int i = 0; i < 10; ++i)
                EXPECT_STREQ("elem", nx_string_array_get(sa, i));
}

} // namespace
