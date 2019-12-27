/**
 * @file nx_data_frame.c
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
#include "virg/nexus/nx_data_frame.h"

#include <limits.h>
#include <stdarg.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_io.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_string.h"
#include "virg/nexus/nx_string_array.h"
#include "virg/nexus/nx_csv_parser.h"

#define NX_DATA_FRAME_DOUBLE_IO_FORMAT "%.17g"

#define CHECK_DATA_COLUMN_TYPE(dc,col_type)                             \
        do {                                                            \
                if (dc->type != col_type)                               \
                        NX_FATAL(NX_LOG_TAG, "Data frame column %d is not of type %s!", \
                                 col_id, #col_type);                    \
        } while (0)

#define CHECK_DATA_FRAME_ELEMENT(dc,row_id,col_type)                    \
        do {                                                            \
                CHECK_DATA_COLUMN_TYPE(dc,col_type);                    \
                if (dc->elems[row_id] == NULL)                          \
                        NX_FATAL(NX_LOG_TAG, "Data frame element at %d,%d is N/A!", \
                                 row_id, col_id);                       \
        } while (0)

#define NA_DATA_FRAME_ELEMENT(dc,row_id)                \
        do {                                            \
                if (dc->elems[row_id] != NULL) {        \
                        nx_free(dc->elems[row_id]);     \
                        dc->elems[row_id] = NULL;       \
                }                                       \
        } while (0)

struct NXDataColumn {
        int size;
        int capacity;
        void **elems;

        enum NXDataColumnType type;
        char *label;

        struct NXStringArray *levels;
};

static struct NXDataColumn *nx_data_column_alloc(enum NXDataColumnType type,
                                                 const char *label)
{
        struct NXDataColumn *dc = NX_NEW(1, struct NXDataColumn);
        dc->size = 0;
        dc->capacity = 0;
        dc->elems = NULL;
        dc->type = type;
        dc->label = nx_strdup(label);
        if (dc->type == NX_DCT_FACTOR)
                dc->levels = nx_string_array_alloc();
        else
                dc->levels = NULL;
        return dc;
}

static void nx_data_column_grow(struct NXDataColumn *dc, int new_size)
{
        NX_ASSERT(dc);

        if (new_size > dc->size) {
                NX_ENSURE_CAPACITY(dc->elems,dc->capacity,new_size);
                for (int i = dc->size; i < new_size; ++i)
                        dc->elems[i] = NULL;
                dc->size = new_size;
        }
}

static struct NXDataColumn *nx_data_column_new(enum NXDataColumnType type,
                                               const char *label, int initial_size)
{
        struct NXDataColumn *dc = nx_data_column_alloc(type, label);
        nx_data_column_grow(dc, initial_size);
        return dc;
}


static void nx_data_column_append_row(struct NXDataColumn *dc)
{
        if (dc->size < INT_MAX)
                nx_data_column_grow(dc,dc->size+1);
        else
                NX_FATAL(NX_LOG_TAG, "Maximum number of rows (%d) has been reached for data frame column %s!",
                         dc->size, dc->label);
}

static void nx_data_column_free(struct NXDataColumn *dc)
{
        if (dc != NULL) {
                for (int i = 0; i < dc->size; ++i)
                        nx_free(dc->elems[i]);
                nx_free(dc->label);
                nx_string_array_free(dc->levels);
                if (dc) {
                        nx_free(dc->elems);
                        nx_free(dc);
                }
        }
}


static int nx_data_column_value_to_factor(struct NXDataColumn *dc,
                                          const char *value)
{
        int level_id = nx_string_array_find(dc->levels, value);
        if (level_id < 0) {
                level_id = nx_string_array_append(dc->levels, value);
        }
        return level_id;
}

struct NXDataColumnArray {
        int size;
        int capacity;
        struct NXDataColumn **elems;
};

struct NXDataFrame {
        int n_rows;
        struct NXDataColumnArray *columns;
};

struct NXDataFrame *nx_data_frame_alloc()
{
        struct NXDataFrame *df = NX_NEW(1, struct NXDataFrame);
        df->n_rows = 0;
        df->columns = NX_NEW(1, struct NXDataColumnArray);
        df->columns->size = 0;
        df->columns->capacity = 0;
        df->columns->elems = NULL;

        return df;
}

void nx_data_frame_free(struct NXDataFrame *df)
{
        if (df != NULL) {
                for (int i = 0; i < df->columns->size; ++i)
                        nx_data_column_free(df->columns->elems[i]);
                if (df->columns) {
                        nx_free(df->columns->elems);
                        nx_free(df->columns);
                }
                nx_free(df);
        }
}

void nx_data_frame_add_column(struct NXDataFrame *df,
                              enum NXDataColumnType type,
                              const char *label)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_PTR(label);

        struct NXDataColumn *dc = nx_data_column_new(type,label,df->n_rows);
        NX_ENSURE_CAPACITY(df->columns->elems, df->columns->capacity, df->columns->size+1);
        df->columns->elems[df->columns->size++] = dc;
}

void nx_data_frame_add_columns(struct NXDataFrame *df,
                               int n,
                               const enum NXDataColumnType *types,
                               const char **labels)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_PTR(types);
        NX_ASSERT_PTR(labels);

        for (int i = 0; i < n; ++i)
                nx_data_frame_add_column(df, types[i], labels[i]);
}

void nx_data_frame_make_factor(struct NXDataFrame *df, int col_id)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(col_id,nx_data_frame_n_columns(df));

        struct NXDataColumn *dc = df->columns->elems[col_id];
        if (dc->type == NX_DCT_FACTOR)
                return;

        df->columns->elems[col_id] = nx_data_column_new(NX_DCT_FACTOR,
                                                        dc->label, df->n_rows);
        if (dc->type == NX_DCT_STRING) {
                for (int i = 0; i < df->n_rows; ++i) {
                        void *elem_ptr = dc->elems[i];
                        if (elem_ptr)
                                nx_data_frame_set_factor(df, i, col_id,
                                                         (const char *)elem_ptr);
                }
        } else {
                for (int i = 0; i < df->n_rows; ++i) {
                        void *elem_ptr = dc->elems[i];
                        if (elem_ptr) {
                                char value[256];
                                switch (dc->type) {
                                case NX_DCT_INT: snprintf(&value[0], 256, "%d", *((int *)elem_ptr)); break;
                                case NX_DCT_DOUBLE: snprintf(&value[0], 256, NX_DATA_FRAME_DOUBLE_IO_FORMAT, *((double *)elem_ptr)); break;
                                case NX_DCT_BOOL: snprintf(&value[0], 256, "%s", (*((NXBool *)elem_ptr)) ? "true" : "false"); break;
                                default: NX_FATAL(NX_LOG_TAG, "Can not make factor from data frame column %s", dc->label);
                                }
                                nx_data_frame_set_factor(df, i, col_id, value);
                        }
                }
        }

        nx_data_column_free(dc);
}

int nx_data_frame_n_columns(const struct NXDataFrame *df)
{
        NX_ASSERT_PTR(df);
        return df->columns->size;
}

const struct NXDataColumn *nx_data_frame_column(const struct NXDataFrame *df,
                                                int col_id)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(col_id,nx_data_frame_n_columns(df));
        return df->columns->elems[col_id];
}

enum NXDataColumnType nx_data_column_type(const struct NXDataColumn *dc)
{
        NX_ASSERT_PTR(dc);
        return dc->type;
}

const char *nx_data_column_label(const struct NXDataColumn *dc)
{
        NX_ASSERT_PTR(dc);
        return dc->label;
}

int nx_data_column_n_factor_levels(const struct NXDataColumn *dc)
{
        NX_ASSERT_PTR(dc);
        if (dc->type == NX_DCT_FACTOR)
                return nx_string_array_size(dc->levels);
        else
                return 0;
}

const char *nx_data_column_factor_level(const struct NXDataColumn *dc, int level_id)
{
        NX_ASSERT_PTR(dc);
        NX_ASSERT(dc->type == NX_DCT_FACTOR);
        NX_ASSERT_INDEX(level_id,nx_data_column_n_factor_levels(dc));

        return nx_string_array_get(dc->levels, level_id);
}

int nx_data_frame_add_row(struct NXDataFrame *df) {
        NX_ASSERT_PTR(df);

        int nc = nx_data_frame_n_columns(df);
        for (int i = 0; i < nc; ++i)
                nx_data_column_append_row(df->columns->elems[i]);

        int new_row_id = df->n_rows++;
        return new_row_id;
}

void nx_data_frame_vset(struct NXDataFrame *df, int row_id, va_list args);

int nx_data_frame_append_row(struct NXDataFrame *df, ...)
{
        NX_ASSERT_PTR(df);

        int row_id = nx_data_frame_add_row(df);
        va_list args;
        va_start(args, df);
        nx_data_frame_vset(df, row_id, args);
        va_end(args);
}

int nx_data_frame_n_rows(const struct NXDataFrame *df)
{
        NX_ASSERT_PTR(df);
        return df->n_rows;
}

NXBool nx_data_frame_is_na(const struct NXDataFrame *df, int row_id, int col_id)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(row_id,df->n_rows);
        NX_ASSERT_INDEX(col_id,nx_data_frame_n_columns(df));

        const struct NXDataColumn *dc = nx_data_frame_column(df, col_id);
        return dc->elems[row_id] == NULL;
}

const char *nx_data_frame_get_string(const struct NXDataFrame *df, int row_id, int col_id)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(row_id,df->n_rows);
        NX_ASSERT_INDEX(col_id,nx_data_frame_n_columns(df));

        const struct NXDataColumn *dc = nx_data_frame_column(df, col_id);
        CHECK_DATA_FRAME_ELEMENT(dc,row_id,NX_DCT_STRING);
        return (const char *)dc->elems[row_id];
}

int nx_data_frame_get_int(const struct NXDataFrame *df, int row_id, int col_id)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(row_id,df->n_rows);
        NX_ASSERT_INDEX(col_id,nx_data_frame_n_columns(df));

        const struct NXDataColumn *dc = nx_data_frame_column(df, col_id);
        CHECK_DATA_FRAME_ELEMENT(dc,row_id,NX_DCT_INT);
        return *((int *)dc->elems[row_id]);
}

double nx_data_frame_get_double(const struct NXDataFrame *df, int row_id, int col_id)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(row_id,df->n_rows);
        NX_ASSERT_INDEX(col_id,nx_data_frame_n_columns(df));

        const struct NXDataColumn *dc = nx_data_frame_column(df, col_id);
        CHECK_DATA_FRAME_ELEMENT(dc,row_id,NX_DCT_DOUBLE);
        return *((double *)dc->elems[row_id]);
}

NXBool nx_data_frame_get_bool(const struct NXDataFrame *df, int row_id, int col_id)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(row_id,df->n_rows);
        NX_ASSERT_INDEX(col_id,nx_data_frame_n_columns(df));

        const struct NXDataColumn *dc = nx_data_frame_column(df, col_id);
        CHECK_DATA_FRAME_ELEMENT(dc,row_id,NX_DCT_BOOL);
        return *((NXBool *)dc->elems[row_id]);
}

const char *nx_data_frame_get_factor(const struct NXDataFrame *df,
                                     int row_id, int col_id)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(row_id,df->n_rows);
        NX_ASSERT_INDEX(col_id,nx_data_frame_n_columns(df));

        const struct NXDataColumn *dc = nx_data_frame_column(df, col_id);
        CHECK_DATA_FRAME_ELEMENT(dc,row_id,NX_DCT_FACTOR);
        int factor_id = *((int *)dc->elems[row_id]);
        return nx_string_array_get(dc->levels, factor_id);
}

void nx_data_frame_vset(struct NXDataFrame *df, int row_id, va_list args)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(row_id,df->n_rows);

        for (int col_id = 0; col_id < df->columns->size; ++col_id) {
                switch (df->columns->elems[col_id]->type) {
                case NX_DCT_INT:
                        nx_data_frame_set_int(df, row_id, col_id,
                                              va_arg(args, int));
                        break;
                case NX_DCT_BOOL:
                        nx_data_frame_set_bool(df, row_id, col_id,
                                               va_arg(args, NXBool));
                        break;
                case NX_DCT_DOUBLE:
                        nx_data_frame_set_double(df, row_id, col_id,
                                                 va_arg(args, double));
                        break;
                case NX_DCT_STRING:
                        nx_data_frame_set_string(df, row_id, col_id,
                                                 va_arg(args, char *));
                        break;
                case NX_DCT_FACTOR:
                        nx_data_frame_set_factor(df, row_id, col_id,
                                                 va_arg(args, char *));
                        break;
                }
        }
}

void nx_data_frame_set(struct NXDataFrame *df, int row_id, ...)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(row_id,df->n_rows);

        va_list args;
        va_start(args, row_id);
        nx_data_frame_vset(df, row_id, args);
        va_end(args);
}

void nx_data_frame_set_na(struct NXDataFrame *df, int row_id, int col_id)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(row_id,df->n_rows);
        NX_ASSERT_INDEX(col_id,nx_data_frame_n_columns(df));

        struct NXDataColumn *dc = df->columns->elems[col_id];
        NA_DATA_FRAME_ELEMENT(dc,row_id);
}

void nx_data_frame_set_string(struct NXDataFrame *df, int row_id, int col_id,
                              const char *value)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(row_id,df->n_rows);
        NX_ASSERT_INDEX(col_id,nx_data_frame_n_columns(df));
        NX_ASSERT_PTR(value);

        struct NXDataColumn *dc = df->columns->elems[col_id];
        CHECK_DATA_COLUMN_TYPE(dc,NX_DCT_STRING);
        NA_DATA_FRAME_ELEMENT(dc,row_id);
        dc->elems[row_id] = nx_strdup(value);
}

void nx_data_frame_set_int(struct NXDataFrame *df, int row_id, int col_id,
                           int value)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(row_id,df->n_rows);
        NX_ASSERT_INDEX(col_id,nx_data_frame_n_columns(df));

        struct NXDataColumn *dc = df->columns->elems[col_id];
        CHECK_DATA_COLUMN_TYPE(dc,NX_DCT_INT);
        NA_DATA_FRAME_ELEMENT(dc,row_id);
        int *elem_ptr = NX_NEW_I(1);
        *elem_ptr = value;
        dc->elems[row_id] = (void *)elem_ptr;
}

void nx_data_frame_set_double(struct NXDataFrame *df, int row_id, int col_id,
                              double value)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(row_id,df->n_rows);
        NX_ASSERT_INDEX(col_id,nx_data_frame_n_columns(df));

        struct NXDataColumn *dc = df->columns->elems[col_id];
        CHECK_DATA_COLUMN_TYPE(dc,NX_DCT_DOUBLE);
        NA_DATA_FRAME_ELEMENT(dc,row_id);
        double *elem_ptr = NX_NEW_D(1);
        *elem_ptr = value;
        dc->elems[row_id] = (void *)elem_ptr;
}

void nx_data_frame_set_bool(struct NXDataFrame *df, int row_id, int col_id,
                            NXBool value)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(row_id,df->n_rows);
        NX_ASSERT_INDEX(col_id,nx_data_frame_n_columns(df));

        struct NXDataColumn *dc = df->columns->elems[col_id];
        CHECK_DATA_COLUMN_TYPE(dc,NX_DCT_BOOL);
        NA_DATA_FRAME_ELEMENT(dc,row_id);
        NXBool *elem_ptr = NX_NEW(1, NXBool);
        *elem_ptr = value;
        dc->elems[row_id] = (void *)elem_ptr;
}

void nx_data_frame_set_factor(struct NXDataFrame *df, int row_id, int col_id,
                              const char *value)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(row_id,df->n_rows);
        NX_ASSERT_INDEX(col_id,nx_data_frame_n_columns(df));
        NX_ASSERT_PTR(value);

        struct NXDataColumn *dc = df->columns->elems[col_id];
        CHECK_DATA_COLUMN_TYPE(dc,NX_DCT_FACTOR);
        NA_DATA_FRAME_ELEMENT(dc,row_id);

        int level_id = nx_data_column_value_to_factor(dc, value);
        int *elem_ptr = NX_NEW_I(1);
        *elem_ptr = level_id;
        dc->elems[row_id] = (void *)elem_ptr;
}

NXBool nx_data_frame_fprintf(const struct NXDataFrame *df, FILE *stream,
                             const char *col_sep)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_PTR(stream);
        NX_ASSERT_PTR(col_sep);

        int nc = nx_data_frame_n_columns(df);
        for (int i = 0; i < nc; ++i) {
                const struct NXDataColumn *dc = nx_data_frame_column(df, i);
                if (EOF == nx_fputs_readable(nx_data_column_label(dc), stream)) {
                        NX_ERROR(NX_LOG_TAG, "Error printing label of data frame for column %d", i);
                        return NX_FALSE;
                }
                if (i != nc-1) {
                        if (EOF == fputs(col_sep, stream)) {
                                NX_ERROR(NX_LOG_TAG, "Error printing seperator of data frame for column %d", i);
                                return NX_FALSE;
                        }
                } else {
                        if (EOF == fputs("\n", stream)) {
                                NX_ERROR(NX_LOG_TAG, "Error printing newline at the end of data frame column labels");
                                return NX_FALSE;
                        }
                }
        }

        int nr = nx_data_frame_n_rows(df);
        for (int r = 0; r < nr; ++r) {
                for (int c = 0; c < nc; ++c) {
                        if (!nx_data_frame_is_na(df, r, c)) {
                                const struct NXDataColumn *dc = nx_data_frame_column(df, c);
                                switch (nx_data_column_type(dc)) {
                                case NX_DCT_STRING:
                                        if (EOF == nx_fputs_double_quoted(nx_data_frame_get_string(df,r,c),
                                                                          stream)) {
                                                NX_ERROR(NX_LOG_TAG, "Error printing data frame string at %d, %d", r, c);
                                                return NX_FALSE;
                                        }
                                        break;
                                case NX_DCT_FACTOR:
                                        if (EOF == nx_fputs_double_quoted(nx_data_frame_get_factor(df,r,c),
                                                                          stream)) {
                                                NX_ERROR(NX_LOG_TAG, "Error printing data frame factor at %d, %d", r, c);
                                                return NX_FALSE;
                                        }
                                        break;
                                case NX_DCT_INT:
                                        if (0 > fprintf(stream, "%d",
                                                        nx_data_frame_get_int(df,r,c))) {
                                                NX_ERROR(NX_LOG_TAG, "Error printing data frame integer at %d, %d", r, c);
                                                return NX_FALSE;
                                        }
                                        break;
                                case NX_DCT_DOUBLE:
                                        if (0 > fprintf(stream,
                                                NX_DATA_FRAME_DOUBLE_IO_FORMAT,
                                                nx_data_frame_get_double(df,r,c))) {
                                                NX_ERROR(NX_LOG_TAG, "Error printing data frame double at %d, %d", r, c);
                                                return NX_FALSE;
                                        }
                                        break;
                                case NX_DCT_BOOL:
                                        if (0 > fprintf(stream, "%s",
                                                        nx_data_frame_get_bool(df,r,c)
                                                        ? "true" : "false")) {
                                                NX_ERROR(NX_LOG_TAG, "Error printing data frame boolean at %d, %d", r, c);
                                                return NX_FALSE;
                                        }
                                        break;
                                }
                        }

                        if (c != nc-1) {
                                if (EOF == fputs(col_sep, stream)) {
                                        NX_ERROR(NX_LOG_TAG, "Error printing seperator of data frame item at %d, %d", r, c);
                                        return NX_FALSE;
                                }
                        } else {
                                if (EOF == fputs("\n", stream)) {
                                        NX_ERROR(NX_LOG_TAG, "Error printing newline at the end of data frame items at row %d", r);
                                        return NX_FALSE;
                                }
                        }
                }
        }

        return NX_TRUE;
}


NXBool nx_data_frame_save_csv_stream(const struct NXDataFrame *df, FILE *stream)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_PTR(stream);

        return nx_data_frame_fprintf(df, stream, ",");
}

struct NXDataFrame *nx_data_frame_load_csv_stream(FILE *stream,
                                                  NXBool strings_as_factors)
{
        NX_ASSERT_PTR(stream);

        int cap = 1024;
        char *csv_text = NX_NEW_C(cap);

        int c;
        int i = 0;
        while ((c = fgetc(stream)) != EOF) {
                csv_text[i++] = c;

                if (i >= cap-2) {
                        cap *= 2;
                        csv_text = nx_xrealloc(csv_text, cap);
                }
        }
        csv_text[i] = '\0';

        struct NXCSVLexer *clex = nx_csv_lexer_new(csv_text);
        struct NXCSVParser *cp = nx_csv_parser_new(clex);
        struct NXDataFrame *df = nx_csv_parser_parse(cp);

        nx_csv_parser_free(cp);
        nx_csv_lexer_free(clex);
        nx_free(csv_text);

        return df;
}

NXBool nx_data_frame_save_csv(const struct NXDataFrame *df, const char *filename)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_PTR(filename);

        FILE *fout = nx_fopen(filename, "w");
        if (fout == NULL) {
                NX_ERROR(NX_LOG_TAG, "Error opening file %s for CSV data frame output",
                         filename);
                return NX_FALSE;
        }

        NXBool res = nx_data_frame_save_csv_stream(df, fout);

        if (nx_fclose(fout, filename) != NX_OK)
                res = NX_FALSE;

        return res;
}

struct NXDataFrame *nx_data_frame_load_csv(const char *filename,
                                           NXBool strings_as_factors)
{
        NX_ASSERT_PTR(filename);

        FILE *fin = nx_fopen(filename, "r");
        if (fin == NULL) {
                NX_ERROR(NX_LOG_TAG, "Error opening file %s for reading CSV data",
                         filename);
                return NX_FALSE;
        }

        struct NXDataFrame *df = nx_data_frame_load_csv_stream(fin,
                                                               strings_as_factors);

        if (nx_fclose(fin, filename) != NX_OK) {
                nx_data_frame_free(df);
                df = NULL;
        }

        return df;
}

void nx_data_frame_xsave_csv(const struct NXDataFrame *df, const char *filename)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_PTR(filename);

        if (!nx_data_frame_save_csv(df, filename))
                NX_FATAL(NX_LOG_TAG, "Error saving data frame to CSV file %s",
                         filename);
}

struct NXDataFrame *nx_data_frame_xload_csv(const char *filename,
                                            NXBool strings_as_factors)
{
        NX_ASSERT_PTR(filename);

        struct NXDataFrame *df = nx_data_frame_load_csv(filename,
                                                        strings_as_factors);
        if (df == NULL)
                NX_FATAL(NX_LOG_TAG, "Error loading data frame from file %s",
                         filename);

        return df;
}
