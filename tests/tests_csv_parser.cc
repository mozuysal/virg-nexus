/**
 * @file tests_csv_parser.cc
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
#include "virg/nexus/nx_csv_parser.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_string.h"

using namespace std;

namespace {

class NXCSVParserTest : public ::testing::Test {
protected:
        NXCSVParserTest() {
        }

        virtual void SetUp() {
        }

        virtual void TearDown() {
        }
};

TEST_F(NXCSVParserTest, new_free) {
        char *s = nx_strdup("");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);
        struct NXCSVParser *cparse = nx_csv_parser_new(clex);

        nx_csv_parser_free(cparse);
        nx_csv_lexer_free(clex);
        nx_free(s);
}

TEST_F(NXCSVParserTest, parse_single_column_label) {
        char *s = nx_strdup("abcd");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);
        struct NXCSVParser *cparse = nx_csv_parser_new(clex);

        struct NXDataFrame *df = nx_csv_parser_parse(cparse);
        EXPECT_EQ(0,nx_data_frame_n_rows(df));
        EXPECT_EQ(1,nx_data_frame_n_columns(df));
        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_EQ(NX_DCT_STRING,nx_data_column_type(dc));
        EXPECT_STREQ("abcd",nx_data_column_label(dc));

        nx_data_frame_free(df);
        nx_csv_parser_free(cparse);
        nx_csv_lexer_free(clex);
        nx_free(s);
}

TEST_F(NXCSVParserTest, parse_multi_column_label) {
        char *s = nx_strdup("abcd,\"cd\"\"ef\",\"qw\nwe\",");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);
        struct NXCSVParser *cparse = nx_csv_parser_new(clex);

        struct NXDataFrame *df = nx_csv_parser_parse(cparse);
        EXPECT_EQ(0,nx_data_frame_n_rows(df));
        EXPECT_EQ(4,nx_data_frame_n_columns(df));

        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_EQ(NX_DCT_STRING,nx_data_column_type(dc));
        EXPECT_STREQ("abcd",nx_data_column_label(dc));

        dc = nx_data_frame_column(df,1);
        EXPECT_EQ(NX_DCT_STRING,nx_data_column_type(dc));
        EXPECT_STREQ("cd\"ef",nx_data_column_label(dc));

        dc = nx_data_frame_column(df,2);
        EXPECT_EQ(NX_DCT_STRING,nx_data_column_type(dc));
        EXPECT_STREQ("qw\nwe",nx_data_column_label(dc));

        dc = nx_data_frame_column(df,3);
        EXPECT_EQ(NX_DCT_STRING,nx_data_column_type(dc));
        EXPECT_STREQ("",nx_data_column_label(dc));

        nx_data_frame_free(df);
        nx_csv_parser_free(cparse);
        nx_csv_lexer_free(clex);
        nx_free(s);
}

TEST_F(NXCSVParserTest, parse_single_column_string) {
        char *s = nx_strdup("abcd\ne\nf\ng\nh");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);
        struct NXCSVParser *cparse = nx_csv_parser_new(clex);

        struct NXDataFrame *df = nx_csv_parser_parse(cparse);
        EXPECT_EQ(4,nx_data_frame_n_rows(df));
        EXPECT_EQ(1,nx_data_frame_n_columns(df));
        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_EQ(NX_DCT_STRING,nx_data_column_type(dc));
        EXPECT_STREQ("abcd",nx_data_column_label(dc));

        EXPECT_STREQ("e",nx_data_frame_get_string(df, 0, 0));
        EXPECT_STREQ("f",nx_data_frame_get_string(df, 1, 0));
        EXPECT_STREQ("g",nx_data_frame_get_string(df, 2, 0));
        EXPECT_STREQ("h",nx_data_frame_get_string(df, 3, 0));

        nx_data_frame_free(df);
        nx_csv_parser_free(cparse);
        nx_csv_lexer_free(clex);
        nx_free(s);
}

TEST_F(NXCSVParserTest, parse_single_column_mixed_string) {
        char *s = nx_strdup("abcd\n1\n1.2\ntrue\nh");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);
        struct NXCSVParser *cparse = nx_csv_parser_new(clex);

        struct NXDataFrame *df = nx_csv_parser_parse(cparse);
        EXPECT_EQ(4,nx_data_frame_n_rows(df));
        EXPECT_EQ(1,nx_data_frame_n_columns(df));
        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_EQ(NX_DCT_STRING,nx_data_column_type(dc));
        EXPECT_STREQ("abcd",nx_data_column_label(dc));

        EXPECT_STREQ("1",nx_data_frame_get_string(df, 0, 0));
        EXPECT_STREQ("1.2",nx_data_frame_get_string(df, 1, 0));
        EXPECT_STREQ("true",nx_data_frame_get_string(df, 2, 0));
        EXPECT_STREQ("h",nx_data_frame_get_string(df, 3, 0));

        nx_data_frame_free(df);
        nx_csv_parser_free(cparse);
        nx_csv_lexer_free(clex);
        nx_free(s);
}

TEST_F(NXCSVParserTest, parse_single_column_int) {
        char *s = nx_strdup("abcd\n12\n34\n45\n56");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);
        struct NXCSVParser *cparse = nx_csv_parser_new(clex);

        struct NXDataFrame *df = nx_csv_parser_parse(cparse);
        EXPECT_EQ(4,nx_data_frame_n_rows(df));
        EXPECT_EQ(1,nx_data_frame_n_columns(df));
        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_EQ(NX_DCT_INT,nx_data_column_type(dc));
        EXPECT_STREQ("abcd",nx_data_column_label(dc));

        EXPECT_EQ(12,nx_data_frame_get_int(df, 0, 0));
        EXPECT_EQ(34,nx_data_frame_get_int(df, 1, 0));
        EXPECT_EQ(45,nx_data_frame_get_int(df, 2, 0));
        EXPECT_EQ(56,nx_data_frame_get_int(df, 3, 0));

        nx_data_frame_free(df);
        nx_csv_parser_free(cparse);
        nx_csv_lexer_free(clex);
        nx_free(s);
}

TEST_F(NXCSVParserTest, parse_single_column_double) {
        char *s = nx_strdup("abcd\n12.0\n34.\n45.2\n56.3");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);
        struct NXCSVParser *cparse = nx_csv_parser_new(clex);

        struct NXDataFrame *df = nx_csv_parser_parse(cparse);
        EXPECT_EQ(4,nx_data_frame_n_rows(df));
        EXPECT_EQ(1,nx_data_frame_n_columns(df));
        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_EQ(NX_DCT_DOUBLE,nx_data_column_type(dc));
        EXPECT_STREQ("abcd",nx_data_column_label(dc));

        EXPECT_EQ(12.0,nx_data_frame_get_double(df, 0, 0));
        EXPECT_EQ(34.0,nx_data_frame_get_double(df, 1, 0));
        EXPECT_EQ(45.2,nx_data_frame_get_double(df, 2, 0));
        EXPECT_EQ(56.3,nx_data_frame_get_double(df, 3, 0));

        nx_data_frame_free(df);
        nx_csv_parser_free(cparse);
        nx_csv_lexer_free(clex);
        nx_free(s);
}

TEST_F(NXCSVParserTest, parse_single_column_mixed_double) {
        char *s = nx_strdup("abcd\n12\n34\n45\n56.3");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);
        struct NXCSVParser *cparse = nx_csv_parser_new(clex);

        struct NXDataFrame *df = nx_csv_parser_parse(cparse);
        EXPECT_EQ(4,nx_data_frame_n_rows(df));
        EXPECT_EQ(1,nx_data_frame_n_columns(df));
        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_EQ(NX_DCT_DOUBLE,nx_data_column_type(dc));
        EXPECT_STREQ("abcd",nx_data_column_label(dc));

        EXPECT_EQ(12.0,nx_data_frame_get_double(df, 0, 0));
        EXPECT_EQ(34.0,nx_data_frame_get_double(df, 1, 0));
        EXPECT_EQ(45.0,nx_data_frame_get_double(df, 2, 0));
        EXPECT_EQ(56.3,nx_data_frame_get_double(df, 3, 0));

        nx_data_frame_free(df);
        nx_csv_parser_free(cparse);
        nx_csv_lexer_free(clex);
        nx_free(s);
}

TEST_F(NXCSVParserTest, parse_single_column_bool) {
        char *s = nx_strdup("abcd\ntrue\nfalse\ntrue\nfalse");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);
        struct NXCSVParser *cparse = nx_csv_parser_new(clex);

        struct NXDataFrame *df = nx_csv_parser_parse(cparse);
        EXPECT_EQ(4,nx_data_frame_n_rows(df));
        EXPECT_EQ(1,nx_data_frame_n_columns(df));
        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_EQ(NX_DCT_BOOL,nx_data_column_type(dc));
        EXPECT_STREQ("abcd",nx_data_column_label(dc));

        EXPECT_TRUE (nx_data_frame_get_bool(df, 0, 0));
        EXPECT_FALSE(nx_data_frame_get_bool(df, 1, 0));
        EXPECT_TRUE (nx_data_frame_get_bool(df, 2, 0));
        EXPECT_FALSE(nx_data_frame_get_bool(df, 3, 0));

        nx_data_frame_free(df);
        nx_csv_parser_free(cparse);
        nx_csv_lexer_free(clex);
        nx_free(s);
}

TEST_F(NXCSVParserTest, parse_single_column_mixed_bool_int) {
        char *s = nx_strdup("abcd\ntrue\nfalse\n1\nfalse");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);
        struct NXCSVParser *cparse = nx_csv_parser_new(clex);

        struct NXDataFrame *df = nx_csv_parser_parse(cparse);
        EXPECT_EQ(4,nx_data_frame_n_rows(df));
        EXPECT_EQ(1,nx_data_frame_n_columns(df));
        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_EQ(NX_DCT_STRING,nx_data_column_type(dc));
        EXPECT_STREQ("abcd",nx_data_column_label(dc));

        EXPECT_STREQ("true",nx_data_frame_get_string(df, 0, 0));
        EXPECT_STREQ("false",nx_data_frame_get_string(df, 1, 0));
        EXPECT_STREQ("1",nx_data_frame_get_string(df, 2, 0));
        EXPECT_STREQ("false",nx_data_frame_get_string(df, 3, 0));

        nx_data_frame_free(df);
        nx_csv_parser_free(cparse);
        nx_csv_lexer_free(clex);
        nx_free(s);
}

TEST_F(NXCSVParserTest, parse_multi_column) {
        char *s = nx_strdup("c.int,c.string,\"c.bool\",c.double\n1,,true,2.3\n,,,\n32,\"\n\",false,3.1\n");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);
        struct NXCSVParser *cparse = nx_csv_parser_new(clex);

        struct NXDataFrame *df = nx_csv_parser_parse(cparse);
        EXPECT_EQ(3,nx_data_frame_n_rows(df));
        EXPECT_EQ(4,nx_data_frame_n_columns(df));

        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_EQ(NX_DCT_INT,nx_data_column_type(dc));
        EXPECT_STREQ("c.int",nx_data_column_label(dc));

        dc = nx_data_frame_column(df,1);
        EXPECT_EQ(NX_DCT_STRING,nx_data_column_type(dc));
        EXPECT_STREQ("c.string",nx_data_column_label(dc));

        dc = nx_data_frame_column(df,2);
        EXPECT_EQ(NX_DCT_BOOL,nx_data_column_type(dc));
        EXPECT_STREQ("c.bool",nx_data_column_label(dc));

        dc = nx_data_frame_column(df,3);
        EXPECT_EQ(NX_DCT_DOUBLE,nx_data_column_type(dc));
        EXPECT_STREQ("c.double",nx_data_column_label(dc));

        EXPECT_EQ(1,nx_data_frame_get_int(df, 0, 0));
        EXPECT_TRUE(nx_data_frame_is_na(df, 1, 0));
        EXPECT_EQ(32,nx_data_frame_get_int(df, 2, 0));

        EXPECT_TRUE(nx_data_frame_is_na(df, 0, 1));
        EXPECT_TRUE(nx_data_frame_is_na(df, 1, 1));
        EXPECT_STREQ("\n",nx_data_frame_get_string(df, 2, 1));

        EXPECT_TRUE(nx_data_frame_get_bool(df, 0, 2));
        EXPECT_TRUE(nx_data_frame_is_na(df, 1, 2));
        EXPECT_FALSE(nx_data_frame_get_bool(df, 2, 2));

        EXPECT_EQ(2.3,nx_data_frame_get_double(df, 0, 3));
        EXPECT_TRUE(nx_data_frame_is_na(df, 1, 3));
        EXPECT_EQ(3.1,nx_data_frame_get_double(df, 2, 3));

        nx_data_frame_free(df);
        nx_csv_parser_free(cparse);
        nx_csv_lexer_free(clex);
        nx_free(s);
}

TEST_F(NXCSVParserTest, parse_empty_DEATH) {
        char *s = nx_strdup("");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);
        struct NXCSVParser *cparse = nx_csv_parser_new(clex);

        EXPECT_DEATH({nx_csv_parser_parse(cparse);},"Can not parse empty CSV!");
}


} // namespace
