/**
 * @file nx_data_frame.c
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
#include "virg/nexus/nx_data_frame.h"

#include <limits.h>
#include <stdarg.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_io.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_string.h"
#include "virg/nexus/nx_string_array.h"
#include "virg/nexus/nx_csv_parser.h"

#define CHECK_DATA_COLUMN_TYPE(dc,col_type)                             \
        do {                                                            \
                if (dc->type != col_type)                               \
                        nx_fatal(NX_LOG_TAG, "Data frame column %d is not of type %s!", col_id, #col_type); \
        } while (0)

#define CHECK_DATA_FRAME_ELEMENT(dc,row_id,col_type)                    \
        do {                                                            \
                CHECK_DATA_COLUMN_TYPE(dc,col_type);                    \
                if (dc->elems[row_id] == NULL)                          \
                        nx_fatal(NX_LOG_TAG, "Data frame element at %d,%d is N/A!", row_id, col_id); \
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

static struct NXDataColumn *nx_data_column_alloc(enum NXDataColumnType type, const char *label)
{
        struct NXDataColumn *dc = NULL;
        NX_NEW_ARRAY_STRUCT(dc,elems,5);
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

static struct NXDataColumn *nx_data_column_new(enum NXDataColumnType type, const char *label, int initial_size)
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
                nx_fatal(NX_LOG_TAG, "Maximum number of rows (%d) has been reached for data frame column %s!", dc->size, dc->label);
}

static void nx_data_column_free(struct NXDataColumn *dc)
{
        if (dc != NULL) {
                for (int i = 0; i < dc->size; ++i)
                        nx_free(dc->elems[i]);
                nx_free(dc->label);
                nx_string_array_free(dc->levels);
                NX_FREE_ARRAY_STRUCT(dc,elems);
        }
}


static int nx_data_column_value_to_factor(struct NXDataColumn *dc, const char *value)
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
        NX_NEW_ARRAY_STRUCT(df->columns,elems,3);

        return df;
}

void nx_data_frame_free(struct NXDataFrame *df)
{
        if (df != NULL) {
                for (int i = 0; i < df->columns->size; ++i)
                        nx_data_column_free(df->columns->elems[i]);
                NX_FREE_ARRAY_STRUCT(df->columns,elems);
                nx_free(df);
        }
}

void nx_data_frame_add_column(struct NXDataFrame *df, enum NXDataColumnType type, const char *label)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_PTR(label);

        struct NXDataColumn *dc = nx_data_column_new(type,label,df->n_rows);
        NX_APPEND_TO_ARRAY_STRUCT(df->columns,elems,dc);
}

void nx_data_frame_make_factor(struct NXDataFrame *df, int col_id)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(col_id,nx_data_frame_n_columns(df));

        struct NXDataColumn *dc = df->columns->elems[col_id];
        if (dc->type == NX_DCT_FACTOR)
                return;

        df->columns->elems[col_id] = nx_data_column_new(NX_DCT_FACTOR,dc->label,df->n_rows);
        if (dc->type == NX_DCT_STRING) {
                for (int i = 0; i < df->n_rows; ++i) {
                        void *elem_ptr = dc->elems[i];
                        if (elem_ptr)
                                nx_data_frame_set_factor(df, i, col_id, (const char *)elem_ptr);
                }
        } else {
                for (int i = 0; i < df->n_rows; ++i) {
                        void *elem_ptr = dc->elems[i];
                        if (elem_ptr) {
                                char value[256];
                                switch (dc->type) {
                                case NX_DCT_INT: snprintf(&value[0], 256, "%d", *((int *)elem_ptr)); break;
                                case NX_DCT_DOUBLE: snprintf(&value[0], 256, "%.15g", *((double *)elem_ptr)); break;
                                case NX_DCT_BOOL: snprintf(&value[0], 256, "%s", (*((NXBool *)elem_ptr)) ? "true" : "false"); break;
                                default: nx_fatal(NX_LOG_TAG, "Can not make factor from data frame column %s", dc->label);
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

const struct NXDataColumn *nx_data_frame_column(const struct NXDataFrame *df, int col_id)
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

const char *nx_data_frame_get_factor(const struct NXDataFrame *df, int row_id, int col_id)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(row_id,df->n_rows);
        NX_ASSERT_INDEX(col_id,nx_data_frame_n_columns(df));

        const struct NXDataColumn *dc = nx_data_frame_column(df, col_id);
        CHECK_DATA_FRAME_ELEMENT(dc,row_id,NX_DCT_FACTOR);
        int factor_id = *((int *)dc->elems[row_id]);
        return nx_string_array_get(dc->levels, factor_id);
}

void nx_data_frame_set(struct NXDataFrame *df, int row_id, ...)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_INDEX(row_id,df->n_rows);

        va_list args;
        va_start(args, row_id);
        for (int col_id = 0; col_id < df->columns->size; ++col_id) {
                switch (df->columns->elems[col_id]->type) {
                case NX_DCT_INT:
                        nx_data_frame_set_int(df, row_id, col_id, va_arg(args, int));
                        break;
                case NX_DCT_BOOL:
                        nx_data_frame_set_bool(df, row_id, col_id, va_arg(args, NXBool));
                        break;
                case NX_DCT_DOUBLE:
                        nx_data_frame_set_double(df, row_id, col_id, va_arg(args, double));
                        break;
                case NX_DCT_STRING:
                        nx_data_frame_set_string(df, row_id, col_id, va_arg(args, char *));
                        break;
                case NX_DCT_FACTOR:
                        nx_data_frame_set_factor(df, row_id, col_id, va_arg(args, char *));
                        break;
                }
        }
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

void nx_data_frame_set_string(struct NXDataFrame *df, int row_id, int col_id, const char *value)
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

void nx_data_frame_set_int(struct NXDataFrame *df, int row_id, int col_id, int value)
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

void nx_data_frame_set_double(struct NXDataFrame *df, int row_id, int col_id, double value)
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

void nx_data_frame_set_bool(struct NXDataFrame *df, int row_id, int col_id, NXBool value)
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

void nx_data_frame_set_factor(struct NXDataFrame *df, int row_id, int col_id, const char *value)
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

NXBool nx_data_frame_save_csv(const struct NXDataFrame *df, const char *filename)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_PTR(filename);

        FILE *fout = fopen(filename, "w");
        if (fout == NULL) {
                nx_error(NX_LOG_TAG, "Error opening file %s for CSV data frame output", filename);
                return NX_FALSE;
        }

        int nc = nx_data_frame_n_columns(df);
        for (int i = 0; i < nc; ++i) {
                const struct NXDataColumn *dc = nx_data_frame_column(df, i);
                nx_fputs_readable(nx_data_column_label(dc), fout);
                if (i != nc-1)
                        fputs(",", fout);
                else
                        fputs("\n", fout);
        }

        int nr = nx_data_frame_n_rows(df);
        for (int r = 0; r < nr; ++r) {
                for (int c = 0; c < nc; ++c) {
                        if (!nx_data_frame_is_na(df, r, c)) {
                                const struct NXDataColumn *dc = nx_data_frame_column(df, c);
                                switch (nx_data_column_type(dc)) {
                                case NX_DCT_STRING: nx_fputs_double_quoted(nx_data_frame_get_string(df,r,c), fout); break;
                                case NX_DCT_FACTOR: nx_fputs_double_quoted(nx_data_frame_get_factor(df,r,c), fout); break;
                                case NX_DCT_INT: fprintf(fout, "%d", nx_data_frame_get_int(df,r,c)); break;
                                case NX_DCT_DOUBLE: fprintf(fout, "%.15g", nx_data_frame_get_double(df,r,c)); break;
                                case NX_DCT_BOOL: fprintf(fout, "%s", nx_data_frame_get_bool(df,r,c) ? "true" : "false"); break;
                                }
                        }

                        if (c != nc-1)
                                fputs(",", fout);
                        else
                                fputs("\n", fout);
                }
        }

        fclose(fout);

        return NX_TRUE;
}

struct NXDataFrame *nx_data_frame_load_csv(const char *filename, NXBool strings_as_factors)
{
        FILE *fin = nx_xfopen(filename, "r");

        int cap = 1024;
        char *csv_text = NX_NEW_C(cap);

        int c;
        int i = 0;
        while ((c = fgetc(fin)) != EOF) {
                csv_text[i++] = c;

                if (i >= cap-2) {
                        cap *= 2;
                        csv_text = nx_frealloc(csv_text, cap);
                }
        }
        csv_text[i] = '\0';
        nx_xfclose(fin, filename);

        struct NXCSVLexer *clex = nx_csv_lexer_new(csv_text);
        struct NXCSVParser *cp = nx_csv_parser_new(clex);
        struct NXDataFrame *df = nx_csv_parser_parse(cp);

        nx_csv_parser_free(cp);
        nx_csv_lexer_free(clex);
        nx_free(csv_text);

        return df;
}

void nx_data_frame_xsave_csv(const struct NXDataFrame *df, const char *filename)
{
        NX_ASSERT_PTR(df);
        NX_ASSERT_PTR(filename);

        if (!nx_data_frame_save_csv(df, filename))
                nx_fatal(NX_LOG_TAG, "Error saving data frame to CSV file %s", filename);
}

struct NXDataFrame *nx_data_frame_xload_csv(const char *filename, NXBool strings_as_factors)
{
        NX_ASSERT_PTR(filename);

        struct NXDataFrame *df = nx_data_frame_load_csv(filename, strings_as_factors);
        if (df == NULL)
                nx_fatal(NX_LOG_TAG, "Error loading data frame from file %s", filename);

        return df;
}
