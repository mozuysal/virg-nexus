/**
 * @file nx_csv_parser.c
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
#include "virg/nexus/nx_csv_parser.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_string.h"

static inline enum NXDataColumnType nx_csv_field_type_from_token_text(const char *text)
{
        if (strcmp("true", text) == 0 || strcmp("false", text) == 0)
                return NX_DCT_BOOL;

        char *endptr;
        long lv = strtol(text, &endptr, 0);
        if (*endptr == '\0' && lv <= INT_MAX && lv >= INT_MIN)
                return NX_DCT_INT;

        errno = 0;
        strtod(text, &endptr);
        if (errno != ERANGE && *endptr == '\0')
                return NX_DCT_DOUBLE;

        return NX_DCT_STRING;
}

struct NXCSVField {
        char *text;
        enum NXDataColumnType type;
};

static struct NXCSVField *nx_csv_field_new(struct NXCSVToken *t)
{
        struct NXCSVField *f = NX_NEW(1, struct NXCSVField);
        if (t == NULL) {
                f->text = NULL;
                f->type = NX_DCT_STRING;
        } else {
                f->text = nx_strdup(t->text);
                if (t->type == NX_CTT_QUOTED_FIELD)
                        f->type = NX_DCT_STRING;
                else
                        f->type = nx_csv_field_type_from_token_text(t->text);
        }

        return f;
}

static void nx_csv_field_free(struct NXCSVField *f)
{
        if (f) {
                nx_free(f->text);
                nx_free(f);
        }
}

static inline void nx_csv_field_set_data_frame(const struct NXCSVField *f, struct NXDataFrame *df, int r, int c)
{
        if (f->text == NULL) {
                nx_data_frame_set_na(df, r, c);
        } else {
                const struct NXDataColumn *dc = nx_data_frame_column(df, c);
                enum NXDataColumnType ctype = nx_data_column_type(dc);
                switch (ctype) {
                case NX_DCT_STRING: nx_data_frame_set_string(df, r, c, f->text); break;
                case NX_DCT_BOOL: nx_data_frame_set_bool(df, r, c, strcmp("true", f->text) == 0); break;
                case NX_DCT_DOUBLE: nx_data_frame_set_double(df, r, c, atof(f->text)); break;
                case NX_DCT_INT: nx_data_frame_set_int(df, r, c, atoi(f->text)); break;
                case NX_DCT_FACTOR: nx_data_frame_set_factor(df, r, c, f->text); break;
                }
        }
}

struct NXCSVRecord {
        int size;
        int capacity;
        struct NXCSVField **fields;
};

struct NXCSVData;
static void nx_csv_data_add_record(struct NXCSVData *data, struct NXCSVRecord *r);

static struct NXCSVRecord *nx_csv_record_new(struct NXCSVData *parent)
{
        struct NXCSVRecord *r = NX_NEW(1, struct NXCSVRecord);
        r->size = 0;
        r->capacity = 0;
        r->fields = NULL;
        nx_csv_data_add_record(parent, r);
        return r;
}

static void nx_csv_record_free(struct NXCSVRecord *r)
{
        if (r) {
                for (int i = 0; i < r->size; ++i)
                        nx_csv_field_free(r->fields[i]);
                if (r) {
                        nx_free(r->fields);
                        nx_free(r);
                }
        }
}

static void nx_csv_record_add_field_from_token(struct NXCSVRecord *r, struct NXCSVToken *t)
{
        struct NXCSVField *f = nx_csv_field_new(t);
        NX_ENSURE_CAPACITY(r->fields, r->capacity, r->size+1);
        r->fields[r->size++] = f;
}

struct NXCSVData {
        int size;
        int capacity;
        struct NXCSVRecord **records;

        int n_columns;
};

static struct NXCSVData *nx_csv_data_alloc()
{
        struct NXCSVData *data = NX_NEW(1, struct NXCSVData);
        data->size = 0;
        data->capacity = 0;
        data->records = NULL;
        data->n_columns = 0;

        return data;
}

static void nx_csv_data_free(struct NXCSVData *data)
{
        if (data) {
                for (int i = 0; i < data->size; ++i)
                        nx_csv_record_free(data->records[i]);
                nx_free(data->records);
                nx_free(data);
        }
}

static void nx_csv_data_add_record(struct NXCSVData *data, struct NXCSVRecord *r)
{
        NX_ENSURE_CAPACITY(data->records, data->capacity, data->size+1);
        data->records[data->size++] = r;
}

// STRING > BOOL > DOUBLE > INT, BOOL && (DOUBLE || INT) ==> STRING
static inline enum NXDataColumnType nx_csv_column_pick_larger_type(enum NXDataColumnType t0, enum NXDataColumnType t1)
{
        if (t0 == NX_DCT_STRING || t1 == NX_DCT_STRING)
                return NX_DCT_STRING;

        if (t0 == NX_DCT_BOOL && t1 == NX_DCT_BOOL)
                return NX_DCT_BOOL;
        else if (t0 == NX_DCT_BOOL || t1 == NX_DCT_BOOL)
                return NX_DCT_STRING;

        if (t0 == NX_DCT_DOUBLE || t1 == NX_DCT_DOUBLE)
                return NX_DCT_DOUBLE;

        return NX_DCT_INT;
}

static enum NXDataColumnType *nx_csv_data_get_column_types(const struct NXCSVData *data)
{
        int nr = data->size;
        int nc = data->n_columns;
        enum NXDataColumnType *ctypes = NX_NEW(nc, enum NXDataColumnType);
        NXBool *ctype_is_set = NX_NEW(nc, NXBool);
        for (int i = 0; i < nc; ++i)
                ctype_is_set[i] = NX_FALSE;

        for (int r = 1; r < nr; ++r) { // skip column names
                const struct NXCSVRecord *record = data->records[r];
                for (int c = 0; c < nc; ++c) {
                        const struct NXCSVField *field = record->fields[c];
                        if (field->text != NULL) {
                                if (!ctype_is_set[c])
                                        ctypes[c] = field->type;
                                else
                                        ctypes[c] = nx_csv_column_pick_larger_type(ctypes[c], field->type);
                                ctype_is_set[c] = NX_TRUE;
                        }
                }
        }

        for (int i = 0; i < nc; ++i)
                if (!ctype_is_set[i])
                        ctypes[i] = NX_DCT_STRING;

        nx_free(ctype_is_set);

        return ctypes;
}

static void nx_csv_data_fill_data_frame(const struct NXCSVData *data, struct NXDataFrame *df)
{
        int nr = data->size;
        int nc = data->n_columns;
        for (int r = 1; r < nr; ++r) { // skip column names
                const struct NXCSVRecord *record = data->records[r];
                int row_id = nx_data_frame_add_row(df);
                for (int c = 0; c < nc; ++c) {
                        const struct NXCSVField *field = record->fields[c];
                        nx_csv_field_set_data_frame(field, df, row_id, c);
                }
        }
}

static struct NXDataFrame *nx_csv_data_to_data_frame(struct NXCSVData *data)
{
        NX_ASSERT_PTR(data);
        NX_ASSERT(data->size > 0);
        NX_ASSERT(data->n_columns > 0);

        enum NXDataColumnType *ctypes = nx_csv_data_get_column_types(data);

        struct NXDataFrame *df = nx_data_frame_alloc();
        const struct NXCSVRecord *labelr = data->records[0];
        int nc = data->n_columns;
        for (int i = 0; i < nc; ++i) {
                const char *label = labelr->fields[i]->text;
                nx_data_frame_add_column(df, ctypes[i], label == NULL ? "" : label);
        }
        nx_free(ctypes);

        nx_csv_data_fill_data_frame(data, df);

        return df;
}

struct NXCSVParser
{
        struct NXCSVLexer *clex;
        struct NXCSVToken *token;
};

static inline void nx_csv_parser_consume(struct NXCSVParser *cp)
{
        nx_csv_lexer_next_token(cp->clex, cp->token);
        NX_ASSERT(cp->token->type != NX_CTT_INVALID);
}

struct NXCSVParser *nx_csv_parser_new(struct NXCSVLexer *clex)
{
        NX_ASSERT_PTR(clex);

        struct NXCSVParser *cp = NX_NEW(1, struct NXCSVParser);

        cp->clex = clex;
        cp->token = NX_NEW(1, struct NXCSVToken);
        cp->token->type = NX_CTT_INVALID;
        cp->token->text = NULL;

        nx_csv_parser_consume(cp);

        return cp;
}

void nx_csv_parser_free(struct NXCSVParser *cp)
{
        if (cp != NULL) {
                nx_csv_token_free_data(cp->token);
                nx_free(cp->token);
                nx_free(cp);
        }
}

static inline void nx_csv_parser_match_comma(struct NXCSVParser *cp)
{
        if (cp->token->type == NX_CTT_COMMA) {
                nx_csv_parser_consume(cp);
        } else {
                NX_FATAL(NX_LOG_TAG, "Expecting comma, found %s", nx_csv_token_name(cp->token));
        }
}

static struct NXCSVRecord *nx_csv_parser_parse_record(struct NXCSVParser *cp, struct NXCSVData *data)
{
        struct NXCSVRecord *r = nx_csv_record_new(data);

        if (cp->token->type == NX_CTT_EOR) {
                NX_FATAL(NX_LOG_TAG, "Empty record %d while parsing CSV", data->size);
        } else if (cp->token->type == NX_CTT_COMMA) {
                nx_csv_record_add_field_from_token(r, NULL);
        } else {
                nx_csv_record_add_field_from_token(r, cp->token);
                nx_csv_parser_consume(cp);
        }

        while (cp->token->type != NX_CTT_EOR && cp->token->type != NX_CTT_EOF) {
                nx_csv_parser_match_comma(cp);

                if (cp->token->type == NX_CTT_COMMA || cp->token->type == NX_CTT_EOR || cp->token->type == NX_CTT_EOF) {
                        nx_csv_record_add_field_from_token(r, NULL);
                } else {
                        nx_csv_record_add_field_from_token(r, cp->token);
                        nx_csv_parser_consume(cp);
                }
        }

        if (cp->token->type == NX_CTT_EOR)
                nx_csv_parser_consume(cp);

        return r;
}

static struct NXCSVData *nx_csv_parser_parse_csv(struct NXCSVParser *cp)
{
        struct NXCSVData *data = nx_csv_data_alloc();

        if (cp->token->type == NX_CTT_EOF)
                NX_FATAL(NX_LOG_TAG, "Can not parse empty CSV!");

        while (cp->token->type != NX_CTT_EOF) {
                struct NXCSVRecord *r = nx_csv_parser_parse_record(cp, data);
                if (data->n_columns == 0)
                        data->n_columns = r->size;
                else if (r->size != data->n_columns)
                        NX_FATAL(NX_LOG_TAG, "Error parsing CSV: Number of fields %d of record %d does not match the previous columns %d",
                                 r->size, data->size-1, data->n_columns);
        }

        return data;
}

struct NXDataFrame *nx_csv_parser_parse(struct NXCSVParser *cp)
{
        NX_ASSERT_PTR(cp);

        struct NXCSVData *data = nx_csv_parser_parse_csv(cp);
        struct NXDataFrame *df = nx_csv_data_to_data_frame(data);
        nx_csv_data_free(data);

        return df;
}
