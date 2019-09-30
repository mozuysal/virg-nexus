/**
 * @file tests_string.cc
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
                f = tmpfile();
        }

        virtual void TearDown() {
                nx_free(p);
                nx_free(q);
                fclose(f);
        }

        char *p;
        char *q;
        FILE *f;
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

TEST_F(NXStringTest, strwrite) {
        const char *s = "text";
        int res = nx_strwrite(s, f);
        EXPECT_TRUE(res == 0);
}

TEST_F(NXStringTest, strwrite_empty) {
        const char *s = "";
        int res = nx_strwrite(s, f);
        EXPECT_TRUE(res == 0);
}

TEST_F(NXStringTest, strread) {
        const char *s = "text";
        int res = nx_strwrite(s, f);
        EXPECT_TRUE(res == 0);

        rewind(f);
        char *scpy = NULL;
        res = nx_strread(&scpy, 0, f);
        EXPECT_TRUE(res == 0);
        EXPECT_STREQ(s, scpy);

        free(scpy);
}

TEST_F(NXStringTest, strread_empty) {
        const char *s = "";
        int res = nx_strwrite(s, f);
        EXPECT_TRUE(res == 0);

        rewind(f);
        char *scpy = NULL;
        res = nx_strread(&scpy, 0, f);
        EXPECT_TRUE(res == 0);
        EXPECT_STREQ(s, scpy);

        free(scpy);
}

TEST_F(NXStringTest, string_from_readable) {
        EXPECT_EQ(NULL, nx_xstr_from_readable(0, NULL));

        const char *r = "";
        char *s = nx_xstr_from_readable(0, r);
        EXPECT_EQ(0, strcmp("", s));
        nx_free(s);

        r = "\\\"";
        s = nx_xstr_from_readable(2, r);
        EXPECT_EQ(0, strcmp("\"", s));
        nx_free(s);

        r = "\\\\";
        s = nx_xstr_from_readable(2, r);
        EXPECT_EQ(0, strcmp("\\", s));
        nx_free(s);

        r = "\\\"abcd\\\"";
        s = nx_xstr_from_readable(8, r);
        EXPECT_EQ(0, strcmp("\"abcd\"", s));
        nx_free(s);
}

TEST_F(NXStringTest, string_from_double_quoted) {
        EXPECT_EQ(NULL, nx_xstr_from_double_quoted(0, NULL));

        const char *r = "";
        char *s = nx_xstr_from_double_quoted(0, r);
        EXPECT_EQ(0, strcmp("", s));
        nx_free(s);

        r = "\"\"";
        s = nx_xstr_from_double_quoted(2, r);
        EXPECT_EQ(0, strcmp("\"", s));
        nx_free(s);

        r = "\\\\";
        s = nx_xstr_from_double_quoted(2, r);
        EXPECT_EQ(0, strcmp("\\\\", s));
        nx_free(s);

        r = "\"\"abcd\"\"";
        s = nx_xstr_from_double_quoted(8, r);
        EXPECT_EQ(0, strcmp("\"abcd\"", s));
        nx_free(s);
}

} // namespace
