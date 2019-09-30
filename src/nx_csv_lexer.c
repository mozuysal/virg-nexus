/**
 * @file nx_csv_lexer.c
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
#include "virg/nexus/nx_csv_lexer.h"

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_string.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_lexer.h"

#define NX_CSV_LEXER_ERROR_LINE_COL_INFO "Line %d, Column %d: "
#define NX_CSV_LEXER_ERROR(lx,msg,...)                                 \
        do {                                                            \
                fflush(stdout);                                         \
                int line_no = nx_lexer_line_no((lx)->lex);              \
                int col_no = nx_lexer_column_no((lx)->lex);             \
                NX_FATAL("NXCSVLexer",NX_CSV_LEXER_ERROR_LINE_COL_INFO msg,line_no,col_no,##__VA_ARGS__); \
        } while (0)

static char *g_csv_token_names[] = {
        "INVALID", "EOF", "EOR", ",", "FIELD", "QFIELD"
};

static void nx_csv_token_fill(struct NXCSVToken *t, enum NXCSVTokenType type, int text_length, const char *text_ptr)
{
        t->type = type;
        if (t->text != NULL) {
                nx_free(t->text);
                t->text = NULL;
        }

        if (text_ptr != NULL) {
                t->text = NX_NEW_C(text_length+1);
                strncpy(t->text, text_ptr, text_length);
                t->text[text_length] = '\0';
        }
}

static void nx_csv_token_fill_qfield(struct NXCSVToken *t, int text_length, const char *text_ptr)
{
        t->type = NX_CTT_QUOTED_FIELD;
        if (t->text != NULL) {
                nx_free(t->text);
                t->text = NULL;
        }
        t->text = nx_xstr_from_double_quoted(text_length, text_ptr);
}

void nx_csv_token_free_data(struct NXCSVToken *t)
{
        if (t) {
                nx_free(t->text);
                t->text = NULL;
        }
}

char *nx_csv_token_to_string(const struct NXCSVToken *t)
{
        if (t->type == NX_CTT_FIELD || t->type == NX_CTT_QUOTED_FIELD)
                return nx_fstr("<%s:'%s'>", nx_csv_token_name(t), t->text);
        else
                return nx_fstr("<%s>", nx_csv_token_name(t));
}

const char *nx_csv_token_name(const struct NXCSVToken *t)
{
        return g_csv_token_names[1 + (int)t->type];
}

const char *nx_csv_token_type_name(enum NXCSVTokenType type)
{
        return g_csv_token_names[1 + (int)type];
}

struct NXCSVLexer
{
        struct NXLexer *lex;
};

struct NXCSVLexer *nx_csv_lexer_new(char *csv_text)
{
        struct NXCSVLexer *clex = NX_NEW(1, struct NXCSVLexer);
        clex->lex = nx_lexer_new(csv_text);
        return clex;
}

void nx_csv_lexer_free(struct NXCSVLexer *clex)
{
        if (clex != NULL) {
                nx_lexer_free(clex->lex);
                nx_free(clex);
        }
}

static inline void nx_csv_lexer_match(struct NXCSVLexer *clex, const char *s)
{
        if (!nx_lexer_match(clex->lex, s))
                NX_CSV_LEXER_ERROR(clex,"Expecting '%s'!",s);
}

static void nx_csv_lexer_FIELD(struct NXCSVLexer *clex, struct NXCSVToken *t)
{
        int start = nx_lexer_position(clex->lex);

        int c = nx_lexer_current_char(clex->lex);
        do {
                if (c == '"') {
                        NX_CSV_LEXER_ERROR(clex,"Unquoted fields can not contain %c", c);
                } else {
                        c = nx_lexer_consume(clex->lex);
                }
        } while (c != EOF && c != '\n' && c != ',');

        int end = nx_lexer_position(clex->lex);
        int len = end - start;
        const char *field = nx_lexer_text(clex->lex) + start;
        nx_csv_token_fill(t, NX_CTT_FIELD, len, field);
}

static void nx_csv_lexer_QFIELD(struct NXCSVLexer *clex, struct NXCSVToken *t)
{
        int c = nx_lexer_consume(clex->lex); // skip "
        int start = nx_lexer_position(clex->lex);

        NXBool has_closing_dquote = NX_FALSE;
        do {
                if (c == '"') {
                        c = nx_lexer_consume(clex->lex);
                        if (c != '"') {
                                has_closing_dquote = NX_TRUE;
                                break;
                        }
                }
                c = nx_lexer_consume(clex->lex);
        } while (c != EOF);

        if (!has_closing_dquote)
                NX_CSV_LEXER_ERROR(clex,"Missing closing %c for string", '\"');

        int end = nx_lexer_position(clex->lex)-1;
        int len = end - start;
        const char *field = nx_lexer_text(clex->lex) + start;
        nx_csv_token_fill_qfield(t, len, field);
}

void nx_csv_lexer_next_token(struct NXCSVLexer *clex, struct NXCSVToken *t)
{
        int c = EOF;
        while ((c = nx_lexer_current_char(clex->lex)) != EOF) {
                switch (c) {
                case ',': nx_lexer_consume(clex->lex); nx_csv_token_fill(t, NX_CTT_COMMA, 0, NULL); return;
                case '\n': nx_lexer_consume(clex->lex); nx_csv_token_fill(t, NX_CTT_EOR, 0, NULL); return;
                case '"': ; nx_csv_lexer_QFIELD(clex, t); return;
                default:
                        nx_csv_lexer_FIELD(clex, t); return;
                }
        }

        nx_csv_token_fill(t, NX_CTT_EOF, 0, NULL);
}
