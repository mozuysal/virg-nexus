/**
 * @file nx_data_frame.h
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
#ifndef VIRG_NEXUS_NX_DATA_FRAME_H
#define VIRG_NEXUS_NX_DATA_FRAME_H

#include <stdio.h>

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

NXBool nx_data_frame_save_csv_stream(const struct NXDataFrame *df, FILE *stream);
struct NXDataFrame *nx_data_frame_load_csv_stream(FILE *stream, NXBool strings_as_factors);

NXBool nx_data_frame_save_csv(const struct NXDataFrame *df, const char *filename);
struct NXDataFrame *nx_data_frame_load_csv(const char *filename, NXBool strings_as_factors);

void nx_data_frame_xsave_csv(const struct NXDataFrame *df, const char *filename);
struct NXDataFrame *nx_data_frame_xload_csv(const char *filename, NXBool strings_as_factors);

__NX_END_DECL

#endif
