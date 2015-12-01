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
#include <climits>

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
        EXPECT_EQ(nx_data_frame_n_columns(df),0);
        EXPECT_EQ(nx_data_frame_n_rows(df),0);
        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, add_column_string) {
        struct NXDataFrame *df = nx_data_frame_alloc();

        const char *label = "col.str";
        enum NXDataColumnType type = NX_DCT_STRING;
        nx_data_frame_add_column(df, type, label);
        EXPECT_EQ(nx_data_frame_n_columns(df),1);

        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_TRUE(dc != NULL);
        EXPECT_EQ(nx_data_column_type(dc),type);
        EXPECT_STREQ(nx_data_column_label(dc),label);
        EXPECT_EQ(nx_data_column_n_factor_levels(dc),0);
        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, add_column_int) {
        struct NXDataFrame *df = nx_data_frame_alloc();

        const char *label = "col.int";
        enum NXDataColumnType type = NX_DCT_INT;
        nx_data_frame_add_column(df, type, label);
        EXPECT_EQ(nx_data_frame_n_columns(df),1);

        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_TRUE(dc != NULL);
        EXPECT_EQ(nx_data_column_type(dc),type);
        EXPECT_STREQ(nx_data_column_label(dc),label);
        EXPECT_EQ(nx_data_column_n_factor_levels(dc),0);
        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, add_column_double) {
        struct NXDataFrame *df = nx_data_frame_alloc();

        const char *label = "col.double";
        enum NXDataColumnType type = NX_DCT_DOUBLE;
        nx_data_frame_add_column(df, type, label);
        EXPECT_EQ(nx_data_frame_n_columns(df),1);

        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_TRUE(dc != NULL);
        EXPECT_EQ(nx_data_column_type(dc),type);
        EXPECT_STREQ(nx_data_column_label(dc),label);
        EXPECT_EQ(nx_data_column_n_factor_levels(dc),0);
        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, add_column_bool) {
        struct NXDataFrame *df = nx_data_frame_alloc();

        const char *label = "col.bool";
        enum NXDataColumnType type = NX_DCT_BOOL;
        nx_data_frame_add_column(df, type, label);
        EXPECT_EQ(nx_data_frame_n_columns(df),1);

        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_TRUE(dc != NULL);
        EXPECT_EQ(nx_data_column_type(dc),type);
        EXPECT_STREQ(nx_data_column_label(dc),label);
        EXPECT_EQ(nx_data_column_n_factor_levels(dc),0);
        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, add_column_factor) {
        struct NXDataFrame *df = nx_data_frame_alloc();

        const char *label = "col.factor";
        enum NXDataColumnType type = NX_DCT_FACTOR;
        nx_data_frame_add_column(df, type, label);
        EXPECT_EQ(nx_data_frame_n_columns(df),1);

        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_TRUE(dc != NULL);
        EXPECT_EQ(nx_data_column_type(dc),type);
        EXPECT_STREQ(nx_data_column_label(dc),label);
        EXPECT_EQ(nx_data_column_n_factor_levels(dc),0);
        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, add_column_all) {
        struct NXDataFrame *df = nx_data_frame_alloc();
        const int n_columns = 5;
        const char *labels[n_columns] = { "col.int", "col.bool", "col.factor", "col.string", "col.double"};
        enum NXDataColumnType types[n_columns] = { NX_DCT_INT, NX_DCT_BOOL, NX_DCT_FACTOR, NX_DCT_STRING, NX_DCT_DOUBLE };
        for (int i = 0; i < n_columns; ++i)
                nx_data_frame_add_column(df, types[i], labels[i]);

        EXPECT_EQ(nx_data_frame_n_columns(df),n_columns);
        for (int i = 0; i < n_columns; ++i) {
                const struct NXDataColumn *dc = nx_data_frame_column(df,i);
                EXPECT_TRUE(dc != NULL);
                EXPECT_EQ(nx_data_column_type(dc),types[i]);
                EXPECT_STREQ(nx_data_column_label(dc),labels[i]);
                EXPECT_EQ(nx_data_column_n_factor_levels(dc),0);
        }

        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, add_row) {
        struct NXDataFrame *df = nx_data_frame_alloc();
        const int n_columns = 5;
        const char *labels[n_columns] = { "col.int", "col.bool", "col.factor", "col.string", "col.double"};
        enum NXDataColumnType types[n_columns] = { NX_DCT_INT, NX_DCT_BOOL, NX_DCT_FACTOR, NX_DCT_STRING, NX_DCT_DOUBLE };
        for (int i = 0; i < n_columns; ++i)
                nx_data_frame_add_column(df, types[i], labels[i]);

        int row_id = nx_data_frame_add_row(df);
        EXPECT_EQ(0, row_id);

        EXPECT_EQ(1, nx_data_frame_n_rows(df));
        for (int i = 0; i < n_columns; ++i)
                EXPECT_TRUE(nx_data_frame_is_na(df, row_id, i));

        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, add_rows) {
        struct NXDataFrame *df = nx_data_frame_alloc();
        const int n_columns = 5;
        const char *labels[n_columns] = { "col.int", "col.bool", "col.factor", "col.string", "col.double"};
        enum NXDataColumnType types[n_columns] = { NX_DCT_INT, NX_DCT_BOOL, NX_DCT_FACTOR, NX_DCT_STRING, NX_DCT_DOUBLE };
        for (int i = 0; i < n_columns; ++i)
                nx_data_frame_add_column(df, types[i], labels[i]);

        int n_rows = 20;
        for (int i = 0; i < n_rows; ++i) {
                int row_id = nx_data_frame_add_row(df);
                EXPECT_EQ(i, row_id);
        }

        EXPECT_EQ(n_rows, nx_data_frame_n_rows(df));
        for (int i = 0; i < n_rows; ++i)
                for (int j = 0; j < n_columns; ++j)
                        EXPECT_TRUE(nx_data_frame_is_na(df, i, j));

        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, set_elems) {
        struct NXDataFrame *df = nx_data_frame_alloc();
        const int n_columns = 5;
        const char *labels[n_columns] = { "col.int", "col.bool", "col.factor", "col.string", "col.double"};
        enum NXDataColumnType types[n_columns] = { NX_DCT_INT, NX_DCT_BOOL, NX_DCT_FACTOR, NX_DCT_STRING, NX_DCT_DOUBLE };
        for (int i = 0; i < n_columns; ++i)
                nx_data_frame_add_column(df, types[i], labels[i]);

        int row_id = nx_data_frame_add_row(df);

        int v_int = 42;
        NXBool v_bool = NX_TRUE;
        const char *v_factor = "FL";
        const char *v_string = "STR";
        double v_double = 42.0;
        nx_data_frame_set_int(df, row_id, 0, v_int);
        nx_data_frame_set_bool(df, row_id, 1, v_bool);
        nx_data_frame_set_factor(df, row_id, 2, v_factor);
        nx_data_frame_set_string(df, row_id, 3, v_string);
        nx_data_frame_set_double(df, row_id, 4, v_double);

        EXPECT_EQ(v_int, nx_data_frame_get_int(df, row_id, 0));
        EXPECT_EQ(v_bool, nx_data_frame_get_bool(df, row_id, 1));
        EXPECT_STREQ(v_factor, nx_data_frame_get_factor(df, row_id, 2));
        EXPECT_STREQ(v_string, nx_data_frame_get_string(df, row_id, 3));
        EXPECT_EQ(v_double, nx_data_frame_get_double(df, row_id, 4));

        const struct NXDataColumn *dc = nx_data_frame_column(df, 2);
        EXPECT_EQ(1, nx_data_column_n_factor_levels(dc));
        EXPECT_STREQ(v_factor, nx_data_column_factor_level(dc, 0));

        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, reset_elems) {
        struct NXDataFrame *df = nx_data_frame_alloc();
        const int n_columns = 5;
        const char *labels[n_columns] = { "col.int", "col.bool", "col.factor", "col.string", "col.double"};
        enum NXDataColumnType types[n_columns] = { NX_DCT_INT, NX_DCT_BOOL, NX_DCT_FACTOR, NX_DCT_STRING, NX_DCT_DOUBLE };
        for (int i = 0; i < n_columns; ++i)
                nx_data_frame_add_column(df, types[i], labels[i]);

        int row_id = nx_data_frame_add_row(df);

        int v_int = 42;
        NXBool v_bool = NX_TRUE;
        const char *v_factor = "FL";
        const char *v_string = "STR";
        double v_double = 42.0;
        nx_data_frame_set_int(df, row_id, 0, v_int);
        nx_data_frame_set_bool(df, row_id, 1, v_bool);
        nx_data_frame_set_factor(df, row_id, 2, v_factor);
        nx_data_frame_set_string(df, row_id, 3, v_string);
        nx_data_frame_set_double(df, row_id, 4, v_double);

        v_int = 24;
        v_bool = NX_FALSE;
        v_factor = "2FL2";
        v_string = "2STR2";
        v_double = 24.0;
        nx_data_frame_set_int(df, row_id, 0, v_int);
        nx_data_frame_set_bool(df, row_id, 1, v_bool);
        nx_data_frame_set_factor(df, row_id, 2, v_factor);
        nx_data_frame_set_string(df, row_id, 3, v_string);
        nx_data_frame_set_double(df, row_id, 4, v_double);

        EXPECT_EQ(v_int, nx_data_frame_get_int(df, row_id, 0));
        EXPECT_EQ(v_bool, nx_data_frame_get_bool(df, row_id, 1));
        EXPECT_STREQ(v_factor, nx_data_frame_get_factor(df, row_id, 2));
        EXPECT_STREQ(v_string, nx_data_frame_get_string(df, row_id, 3));
        EXPECT_EQ(v_double, nx_data_frame_get_double(df, row_id, 4));

        const struct NXDataColumn *dc = nx_data_frame_column(df, 2);
        EXPECT_EQ(2, nx_data_column_n_factor_levels(dc));
        EXPECT_STREQ("FL", nx_data_column_factor_level(dc, 0));
        EXPECT_STREQ("2FL2", nx_data_column_factor_level(dc, 1));

        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, reset_elems_na) {
        struct NXDataFrame *df = nx_data_frame_alloc();
        const int n_columns = 5;
        const char *labels[n_columns] = { "col.int", "col.bool", "col.factor", "col.string", "col.double"};
        enum NXDataColumnType types[n_columns] = { NX_DCT_INT, NX_DCT_BOOL, NX_DCT_FACTOR, NX_DCT_STRING, NX_DCT_DOUBLE };
        for (int i = 0; i < n_columns; ++i)
                nx_data_frame_add_column(df, types[i], labels[i]);

        int row_id = nx_data_frame_add_row(df);

        int v_int = 42;
        NXBool v_bool = NX_TRUE;
        const char *v_factor = "FL";
        const char *v_string = "STR";
        double v_double = 42.0;
        nx_data_frame_set_int(df, row_id, 0, v_int);
        nx_data_frame_set_bool(df, row_id, 1, v_bool);
        nx_data_frame_set_factor(df, row_id, 2, v_factor);
        nx_data_frame_set_string(df, row_id, 3, v_string);
        nx_data_frame_set_double(df, row_id, 4, v_double);

        nx_data_frame_set_na(df, row_id, 0);
        nx_data_frame_set_na(df, row_id, 1);
        nx_data_frame_set_na(df, row_id, 2);
        nx_data_frame_set_na(df, row_id, 3);
        nx_data_frame_set_na(df, row_id, 4);

        EXPECT_TRUE(nx_data_frame_is_na(df, row_id, 0));
        EXPECT_TRUE(nx_data_frame_is_na(df, row_id, 1));
        EXPECT_TRUE(nx_data_frame_is_na(df, row_id, 2));
        EXPECT_TRUE(nx_data_frame_is_na(df, row_id, 3));
        EXPECT_TRUE(nx_data_frame_is_na(df, row_id, 4));

        v_int = 24;
        v_bool = NX_FALSE;
        v_factor = "2FL2";
        v_string = "2STR2";
        v_double = 24.0;
        nx_data_frame_set_int(df, row_id, 0, v_int);
        nx_data_frame_set_bool(df, row_id, 1, v_bool);
        nx_data_frame_set_factor(df, row_id, 2, v_factor);
        nx_data_frame_set_string(df, row_id, 3, v_string);
        nx_data_frame_set_double(df, row_id, 4, v_double);

        EXPECT_EQ(v_int, nx_data_frame_get_int(df, row_id, 0));
        EXPECT_EQ(v_bool, nx_data_frame_get_bool(df, row_id, 1));
        EXPECT_STREQ(v_factor, nx_data_frame_get_factor(df, row_id, 2));
        EXPECT_STREQ(v_string, nx_data_frame_get_string(df, row_id, 3));
        EXPECT_EQ(v_double, nx_data_frame_get_double(df, row_id, 4));

        const struct NXDataColumn *dc = nx_data_frame_column(df, 2);
        EXPECT_EQ(2, nx_data_column_n_factor_levels(dc));
        EXPECT_STREQ("FL", nx_data_column_factor_level(dc, 0));
        EXPECT_STREQ("2FL2", nx_data_column_factor_level(dc, 1));

        nx_data_frame_free(df);
}


TEST_F(NXDataFrameTest, set_row) {
        struct NXDataFrame *df = nx_data_frame_alloc();
        const int n_columns = 5;
        const char *labels[n_columns] = { "col.int", "col.bool", "col.factor", "col.string", "col.double"};
        enum NXDataColumnType types[n_columns] = { NX_DCT_INT, NX_DCT_BOOL, NX_DCT_FACTOR, NX_DCT_STRING, NX_DCT_DOUBLE };
        for (int i = 0; i < n_columns; ++i)
                nx_data_frame_add_column(df, types[i], labels[i]);

        int row_id = nx_data_frame_add_row(df);

        int v_int = 42;
        NXBool v_bool = NX_TRUE;
        const char *v_factor = "FL";
        const char *v_string = "STR";
        double v_double = 42.0;
        nx_data_frame_set(df, row_id, v_int, v_bool, v_factor, v_string, v_double);

        EXPECT_EQ(v_int, nx_data_frame_get_int(df, row_id, 0));
        EXPECT_EQ(v_bool, nx_data_frame_get_bool(df, row_id, 1));
        EXPECT_STREQ(v_factor, nx_data_frame_get_factor(df, row_id, 2));
        EXPECT_STREQ(v_string, nx_data_frame_get_string(df, row_id, 3));
        EXPECT_EQ(v_double, nx_data_frame_get_double(df, row_id, 4));

        const struct NXDataColumn *dc = nx_data_frame_column(df, 2);
        EXPECT_EQ(1, nx_data_column_n_factor_levels(dc));
        EXPECT_STREQ(v_factor, nx_data_column_factor_level(dc, 0));

        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, set_rows) {
        struct NXDataFrame *df = nx_data_frame_alloc();
        const int n_columns = 5;
        const char *labels[n_columns] = { "col.int", "col.bool", "col.factor", "col.string", "col.double"};
        enum NXDataColumnType types[n_columns] = { NX_DCT_INT, NX_DCT_BOOL, NX_DCT_FACTOR, NX_DCT_STRING, NX_DCT_DOUBLE };
        for (int i = 0; i < n_columns; ++i)
                nx_data_frame_add_column(df, types[i], labels[i]);

        for (int i = 0; i < n_columns; ++i)
                nx_data_frame_add_row(df);

        int v_int = 42;
        NXBool v_bool = NX_TRUE;
        const char *v_factor = "FL";
        const char *v_string = "STR";
        double v_double = 42.0;
        nx_data_frame_set_int(df, 0, 0, v_int);
        nx_data_frame_set_bool(df, 1, 1, v_bool);
        nx_data_frame_set_factor(df, 2, 2, v_factor);
        nx_data_frame_set_string(df, 3, 3, v_string);
        nx_data_frame_set_double(df, 4, 4, v_double);

        EXPECT_EQ(v_int, nx_data_frame_get_int(df, 0, 0));
        EXPECT_EQ(v_bool, nx_data_frame_get_bool(df, 1, 1));
        EXPECT_STREQ(v_factor, nx_data_frame_get_factor(df, 2, 2));
        EXPECT_STREQ(v_string, nx_data_frame_get_string(df, 3, 3));
        EXPECT_EQ(v_double, nx_data_frame_get_double(df, 4, 4));

        for (int i = 0; i < n_columns; ++i)
                for (int j = 0; j < n_columns; ++j)
                        if (i != j)
                                EXPECT_TRUE(nx_data_frame_is_na(df, i, j));

        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, set_factor) {
        struct NXDataFrame *df = nx_data_frame_alloc();
        const int n_columns = 5;
        const char *labels[n_columns] = { "col.int", "col.bool", "col.factor", "col.string", "col.double"};
        enum NXDataColumnType types[n_columns] = { NX_DCT_INT, NX_DCT_BOOL, NX_DCT_FACTOR, NX_DCT_STRING, NX_DCT_DOUBLE };
        for (int i = 0; i < n_columns; ++i)
                nx_data_frame_add_column(df, types[i], labels[i]);
        const struct NXDataColumn *dc = nx_data_frame_column(df, 2);

        for (int i = 0; i < 20; ++i)
                nx_data_frame_add_row(df);

        for (int i = 0; i < 5; ++i) {
                nx_data_frame_set_factor(df, i, 2, "L1");
                EXPECT_EQ(1, nx_data_column_n_factor_levels(dc));
        }

        for (int i = 5; i < 10; ++i) {
                nx_data_frame_set_factor(df, i, 2, "L2");
                EXPECT_EQ(2, nx_data_column_n_factor_levels(dc));
        }

        for (int i = 10; i < 15; ++i) {
                nx_data_frame_set_factor(df, i, 2, "L1");
                EXPECT_EQ(2, nx_data_column_n_factor_levels(dc));
        }

        for (int i = 15; i < 20; ++i) {
                nx_data_frame_set_factor(df, i, 2, "L3");
                EXPECT_EQ(3, nx_data_column_n_factor_levels(dc));
        }

        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, make_factor_int) {
        struct NXDataFrame *df = nx_data_frame_alloc();

        const char *label = "col.int";
        enum NXDataColumnType type = NX_DCT_INT;
        nx_data_frame_add_column(df, type, label);

        for (int i = 0; i < 5; ++i)
                nx_data_frame_add_row(df);
        nx_data_frame_set_int(df, 0, 0, 1);
        nx_data_frame_set_int(df, 1, 0, 1);
        nx_data_frame_set_int(df, 2, 0, 2);
        nx_data_frame_set_int(df, 3, 0, 3);
        nx_data_frame_set_int(df, 4, 0, 2);

        nx_data_frame_make_factor(df, 0);
        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_EQ(nx_data_column_type(dc),NX_DCT_FACTOR);
        EXPECT_STREQ(nx_data_column_label(dc),label);
        EXPECT_EQ(nx_data_column_n_factor_levels(dc),3);
        EXPECT_STREQ("1", nx_data_column_factor_level(dc, 0));
        EXPECT_STREQ("2", nx_data_column_factor_level(dc, 1));
        EXPECT_STREQ("3", nx_data_column_factor_level(dc, 2));

        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, make_factor_double) {
        struct NXDataFrame *df = nx_data_frame_alloc();

        const char *label = "col.double";
        enum NXDataColumnType type = NX_DCT_DOUBLE;
        nx_data_frame_add_column(df, type, label);

        for (int i = 0; i < 5; ++i)
                nx_data_frame_add_row(df);
        nx_data_frame_set_double(df, 0, 0, 1.0);
        nx_data_frame_set_double(df, 1, 0, 1.0);
        nx_data_frame_set_double(df, 2, 0, 2.0);
        nx_data_frame_set_double(df, 3, 0, 3.0);
        nx_data_frame_set_double(df, 4, 0, 2.0);

        nx_data_frame_make_factor(df, 0);
        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_EQ(nx_data_column_type(dc),NX_DCT_FACTOR);
        EXPECT_STREQ(nx_data_column_label(dc),label);
        EXPECT_EQ(nx_data_column_n_factor_levels(dc),3);
        EXPECT_STREQ("1.000000", nx_data_column_factor_level(dc, 0));
        EXPECT_STREQ("2.000000", nx_data_column_factor_level(dc, 1));
        EXPECT_STREQ("3.000000", nx_data_column_factor_level(dc, 2));

        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, make_factor_bool) {
        struct NXDataFrame *df = nx_data_frame_alloc();

        const char *label = "col.bool";
        enum NXDataColumnType type = NX_DCT_BOOL;
        nx_data_frame_add_column(df, type, label);

        for (int i = 0; i < 5; ++i)
                nx_data_frame_add_row(df);
        nx_data_frame_set_bool(df, 0, 0, NX_FALSE);
        nx_data_frame_set_bool(df, 1, 0, NX_FALSE);
        nx_data_frame_set_bool(df, 2, 0, NX_TRUE);
        nx_data_frame_set_bool(df, 3, 0, NX_FALSE);
        nx_data_frame_set_bool(df, 4, 0, NX_TRUE);

        nx_data_frame_make_factor(df, 0);
        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_EQ(nx_data_column_type(dc),NX_DCT_FACTOR);
        EXPECT_STREQ(nx_data_column_label(dc),label);
        EXPECT_EQ(nx_data_column_n_factor_levels(dc),2);
        EXPECT_STREQ("false", nx_data_column_factor_level(dc, 0));
        EXPECT_STREQ("true", nx_data_column_factor_level(dc, 1));

        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, make_factor_string) {
        struct NXDataFrame *df = nx_data_frame_alloc();

        const char *label = "col.string";
        enum NXDataColumnType type = NX_DCT_STRING;
        nx_data_frame_add_column(df, type, label);

        for (int i = 0; i < 5; ++i)
                nx_data_frame_add_row(df);
        nx_data_frame_set_string(df, 0, 0, "STR0");
        nx_data_frame_set_string(df, 1, 0, "STR1");
        nx_data_frame_set_string(df, 2, 0, "STR1");
        nx_data_frame_set_string(df, 3, 0, "STR0");
        nx_data_frame_set_string(df, 4, 0, "STR2");

        nx_data_frame_make_factor(df, 0);
        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        EXPECT_EQ(nx_data_column_type(dc),NX_DCT_FACTOR);
        EXPECT_STREQ(nx_data_column_label(dc),label);
        EXPECT_EQ(nx_data_column_n_factor_levels(dc),3);
        EXPECT_STREQ("STR0", nx_data_column_factor_level(dc, 0));
        EXPECT_STREQ("STR1", nx_data_column_factor_level(dc, 1));
        EXPECT_STREQ("STR2", nx_data_column_factor_level(dc, 2));

        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, make_factor_factor) {
        struct NXDataFrame *df = nx_data_frame_alloc();

        const char *label = "col.factor";
        enum NXDataColumnType type = NX_DCT_FACTOR;
        nx_data_frame_add_column(df, type, label);

        for (int i = 0; i < 5; ++i)
                nx_data_frame_add_row(df);
        nx_data_frame_set_factor(df, 0, 0, "FACTOR0");
        nx_data_frame_set_factor(df, 1, 0, "FACTOR1");
        nx_data_frame_set_factor(df, 2, 0, "FACTOR1");
        nx_data_frame_set_factor(df, 3, 0, "FACTOR0");
        nx_data_frame_set_factor(df, 4, 0, "FACTOR2");

        const struct NXDataColumn *dc = nx_data_frame_column(df,0);
        nx_data_frame_make_factor(df, 0);
        EXPECT_EQ(dc, nx_data_frame_column(df,0));
        EXPECT_EQ(nx_data_column_type(dc),NX_DCT_FACTOR);
        EXPECT_STREQ(nx_data_column_label(dc),label);
        EXPECT_EQ(nx_data_column_n_factor_levels(dc),3);
        EXPECT_STREQ("FACTOR0", nx_data_column_factor_level(dc, 0));
        EXPECT_STREQ("FACTOR1", nx_data_column_factor_level(dc, 1));
        EXPECT_STREQ("FACTOR2", nx_data_column_factor_level(dc, 2));

        nx_data_frame_free(df);
}

TEST_F(NXDataFrameTest, io) {
        struct NXDataFrame *df = nx_data_frame_alloc();
        const int n_columns = 5;
        const char *labels[n_columns] = { "col.int", "col.bool", "col.factor", "col.string", "col.double"};
        enum NXDataColumnType types[n_columns] = { NX_DCT_INT, NX_DCT_BOOL, NX_DCT_FACTOR, NX_DCT_STRING, NX_DCT_DOUBLE };
        for (int i = 0; i < n_columns; ++i)
                nx_data_frame_add_column(df, types[i], labels[i]);

        int row_id = nx_data_frame_add_row(df);
        nx_data_frame_set(df, row_id, 123, NX_TRUE, "abc\"\\\"def", "123.0", 1.0/3.0);

        row_id = nx_data_frame_add_row(df);
        nx_data_frame_set(df, row_id, -123, NX_FALSE, "", "", 0.33);
        nx_data_frame_set_na(df, row_id, 3);

        row_id = nx_data_frame_add_row(df);
        nx_data_frame_set(df, row_id, INT_MAX, NX_TRUE, "abcd", "efgh", 1.0/3.0 + 3.0);

        nx_data_frame_save_csv(df, "/tmp/df.csv");
        nx_data_frame_free(df);
}

} // namespace
