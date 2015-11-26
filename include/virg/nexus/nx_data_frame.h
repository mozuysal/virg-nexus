/**
 * @file nx_data_frame.h
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
#ifndef VIRG_NEXUS_NX_DATA_FRAME_H
#define VIRG_NEXUS_NX_DATA_FRAME_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

struct NXDataColumn;
struct NXDataFrame;

enum NXDataColumnType {
        NX_DCT_STRING,
        NX_DCT_INT,
        NX_DCT_DOUBLE,
        NX_DCT_BOOL,
        NX_DCT_FACTOR
};

struct NXDataFrame *nx_data_frame_alloc();

void nx_data_frame_free(struct NXDataFrame *df);

void nx_data_frame_add_column(struct NXDataFrame *df, enum NXDataColumnType type, const char *label);

void nx_data_frame_make_factor(struct NXDataFrame *df, int col_id);

int nx_data_frame_n_columns(const struct NXDataFrame *df);

const struct NXDataColumn *nx_data_frame_column(const struct NXDataFrame *df, int col_id);

enum NXDataColumnType nx_data_column_type(const struct NXDataColumn *dc);

const char *nx_data_column_label(const struct NXDataColumn *dc);

int nx_data_column_n_factor_levels(const struct NXDataColumn *dc);

const char *nx_data_column_factor_level(const struct NXDataColumn *dc, int level_id);

int nx_data_frame_add_row(struct NXDataFrame *df);

int nx_data_frame_n_rows(const struct NXDataFrame *df);

NXBool      nx_data_frame_is_na     (const struct NXDataFrame *df, int row_id, int col_id);
const char *nx_data_frame_get_string(const struct NXDataFrame *df, int row_id, int col_id);
int         nx_data_frame_get_int   (const struct NXDataFrame *df, int row_id, int col_id);
double      nx_data_frame_get_double(const struct NXDataFrame *df, int row_id, int col_id);
NXBool      nx_data_frame_get_bool  (const struct NXDataFrame *df, int row_id, int col_id);
const char *nx_data_frame_get_factor(const struct NXDataFrame *df, int row_id, int col_id);

void nx_data_frame_set       (struct NXDataFrame *df, int row_id, ...);
void nx_data_frame_set_na    (struct NXDataFrame *df, int row_id, int col_id);
void nx_data_frame_set_string(struct NXDataFrame *df, int row_id, int col_id, const char *value);
void nx_data_frame_set_int   (struct NXDataFrame *df, int row_id, int col_id, int value);
void nx_data_frame_set_double(struct NXDataFrame *df, int row_id, int col_id, double value);
void nx_data_frame_set_bool  (struct NXDataFrame *df, int row_id, int col_id, NXBool value);
void nx_data_frame_set_factor(struct NXDataFrame *df, int row_id, int col_id, const char *value);

NXBool nx_data_frame_save_csv(const struct NXDataFrame *df, const char *filename);
struct NXDataFrame *nx_data_frame_load_csv(const char *filename, NXBool strings_as_factors);

void nx_data_frame_xsave_csv(const struct NXDataFrame *df, const char *filename);
struct NXDataFrame *nx_data_frame_xload_csv(const char *filename, NXBool strings_as_factors);

__NX_END_DECL

#endif
