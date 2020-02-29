/**
 * @file tests_string_array.cc
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

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_string.h"
#include "virg/nexus/nx_string_array.h"

using namespace std;

namespace {

class NXStringArrayTest : public ::testing::Test {
protected:
        NXStringArrayTest() {
        }

        virtual void SetUp() {
                sa = nx_string_array_alloc();
                f = tmpfile();
        }

        virtual void TearDown() {
                nx_string_array_free(sa);
                fclose(f);
        }

        struct NXStringArray *sa;
        FILE *f;
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
        for (int i = 0; i < 10; ++i) {
                int id = nx_string_array_append(sa, "elem");
                EXPECT_EQ(i, id);
        }

        EXPECT_EQ(10, nx_string_array_size(sa));
        for (int i = 0; i < 10; ++i)
                EXPECT_STREQ("elem", nx_string_array_get(sa, i));
}

TEST_F(NXStringArrayTest, find) {
        for (int i = 0; i < 10; ++i)
                nx_string_array_appendf(sa, "elem%d", i);

        for (int i = 0; i < 10; ++i) {
                char *ss = nx_fstr("elem%d", i);
                EXPECT_EQ(i, nx_string_array_find(sa, ss));
                nx_free(ss);
        }
}

TEST_F(NXStringArrayTest, write) {
        nx_string_array_resize(sa, 2);
        nx_string_array_set(sa, 0, "abcd");
        nx_string_array_set(sa, 1, "efgh");

        int res = nx_string_array_write(sa, f);
        EXPECT_TRUE(res == 0);
}

TEST_F(NXStringArrayTest, write_empty) {
        int res = nx_string_array_write(sa, f);
        EXPECT_TRUE(res == 0);
}

TEST_F(NXStringArrayTest, read) {
        nx_string_array_resize(sa, 2);
        nx_string_array_set(sa, 0, "abcd");
        nx_string_array_set(sa, 1, "efgh");

        int res = nx_string_array_write(sa, f);
        EXPECT_TRUE(res == 0);

        rewind(f);
        res = nx_string_array_read(sa, f);
        EXPECT_TRUE(res == 0);
        EXPECT_EQ(2, nx_string_array_size(sa));
        EXPECT_STREQ("abcd", nx_string_array_get(sa, 0));
        EXPECT_STREQ("efgh", nx_string_array_get(sa, 1));
}

TEST_F(NXStringArrayTest, read_empty) {
        int res = nx_string_array_write(sa, f);
        EXPECT_TRUE(res == 0);

        rewind(f);
        res = nx_string_array_read(sa, f);
        EXPECT_TRUE(res == 0);
        EXPECT_EQ(0, nx_string_array_size(sa));
}

TEST_F(NXStringArrayTest, read0) {
        nx_string_array_resize(sa, 2);
        nx_string_array_set(sa, 0, "abcd");
        nx_string_array_set(sa, 1, "efgh");

        int res = nx_string_array_write(sa, f);
        EXPECT_TRUE(res == 0);

        rewind(f);
        struct NXStringArray *sacpy = nx_string_array_read0(f);
        EXPECT_TRUE(sacpy != NULL);
        EXPECT_EQ(2, nx_string_array_size(sacpy));
        EXPECT_STREQ("abcd", nx_string_array_get(sacpy, 0));
        EXPECT_STREQ("efgh", nx_string_array_get(sacpy, 1));
        nx_string_array_free(sacpy);
}

TEST_F(NXStringArrayTest, read0_empty) {
        int res = nx_string_array_write(sa, f);
        EXPECT_TRUE(res == 0);

        rewind(f);
        struct NXStringArray *sacpy = nx_string_array_read0(f);
        EXPECT_TRUE(sacpy != NULL);
        EXPECT_EQ(0, nx_string_array_size(sacpy));
        nx_string_array_free(sacpy);
}

} // namespace
