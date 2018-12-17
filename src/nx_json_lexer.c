/**
 * @file nx_json_lexer.c
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
#include "virg/nexus/nx_json_lexer.h"

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_string.h"
#include "virg/nexus/nx_message.h"
#include "virg/nexus/nx_lexer.h"

#define NX_JSON_LEXER_ERROR_LINE_COL_INFO "Line %d, Column %d: "
#define NX_JSON_LEXER_ERROR(lx,msg,...)                                 \
        do {                                                            \
                fflush(stdout);                                         \
                int line_no = nx_lexer_line_no((lx)->lex);              \
                int col_no = nx_lexer_column_no((lx)->lex);             \
                NX_FATAL("NXJSONLexer",NX_JSON_LEXER_ERROR_LINE_COL_INFO msg,line_no,col_no,##__VA_ARGS__); \
        } while (0)

static char *g_json_token_names[] = {
        "INVALID", "EOF", "[", "]", "{", "}",
        ",", ":", "true", "false", "null", "INTEGER", "FPNUM", "STRING"
};

static void nx_json_token_fill(struct NXJSONToken *t, enum NXJSONTokenType type, int text_length, const char *text_ptr)
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

static void nx_json_token_fill_string(struct NXJSONToken *t, int text_length, const char *text_ptr)
{
        t->type = NX_JTT_STRING;
        if (t->text != NULL) {
                nx_free(t->text);
                t->text = NULL;
        }
        t->text = nx_xstr_from_readable(text_length, text_ptr);
}

void nx_json_token_free_data(struct NXJSONToken *t)
{
        if (t) {
                nx_free(t->text);
                t->text = NULL;
        }
}

char *nx_json_token_to_string(const struct NXJSONToken *t)
{
        if (t->type == NX_JTT_INTEGER || t->type == NX_JTT_FPNUM || t->type == NX_JTT_STRING)
                return nx_fstr("<%s:'%s'>", nx_json_token_name(t), t->text);
        else
                return nx_fstr("<%s>", nx_json_token_name(t));
}

const char *nx_json_token_name(const struct NXJSONToken *t)
{
        return g_json_token_names[1 + (int)t->type];
}

const char *nx_json_token_type_name(enum NXJSONTokenType type)
{
        return g_json_token_names[1 + (int)type];
}

struct NXJSONLexer
{
        struct NXLexer *lex;
};

struct NXJSONLexer *nx_json_lexer_new(char *json_text)
{
        struct NXJSONLexer *jlex = NX_NEW(1, struct NXJSONLexer);
        jlex->lex = nx_lexer_new(json_text);
        return jlex;
}

void nx_json_lexer_free(struct NXJSONLexer *jlex)
{
        if (jlex != NULL) {
                nx_lexer_free(jlex->lex);
                nx_free(jlex);
        }
}

static NXBool nx_json_lexer_is_whitespace(int c)
{
        if (c == '\n' || c == '\r' || c == '\t' || c == ' ')
                return NX_TRUE;

        return NX_FALSE;
}

static void nx_json_lexer_skip_whitespace(struct NXJSONLexer *jlex)
{
        while (nx_json_lexer_is_whitespace(nx_lexer_current_char(jlex->lex)))
                nx_lexer_consume(jlex->lex);
}

static inline void nx_json_lexer_match(struct NXJSONLexer *jlex, const char *s)
{
        if (!nx_lexer_match(jlex->lex, s))
                NX_JSON_LEXER_ERROR(jlex,"Expecting '%s'!",s);
}

static void nx_json_lexer_TRUE(struct NXJSONLexer *jlex, struct NXJSONToken *t)
{
        nx_lexer_consume(jlex->lex);
        nx_json_lexer_match(jlex, "rue");
        nx_json_token_fill(t, NX_JTT_TRUE, 0, NULL);
}

static void nx_json_lexer_FALSE(struct NXJSONLexer *jlex, struct NXJSONToken *t)
{
        nx_lexer_consume(jlex->lex);
        nx_json_lexer_match(jlex, "alse");
        nx_json_token_fill(t, NX_JTT_FALSE, 0, NULL);
}

static void nx_json_lexer_NULL(struct NXJSONLexer *jlex, struct NXJSONToken *t)
{
        nx_lexer_consume(jlex->lex);
        nx_json_lexer_match(jlex, "ull");
        nx_json_token_fill(t, NX_JTT_NULL, 0, NULL);
}

static void nx_json_lexer_NUMBER(struct NXJSONLexer *jlex, struct NXJSONToken *t)
{
        int len;
        enum NXLexerNumberType type;
        const char * res = nx_lexer_NUMBER(jlex->lex, &len, &type);

        switch(type) {
        case NX_LNT_INTEGER: nx_json_token_fill(t, NX_JTT_INTEGER, len, res); return;
        case NX_LNT_FLOATING_POINT: nx_json_token_fill(t, NX_JTT_FPNUM, len, res); return;
        default:
                NX_JSON_LEXER_ERROR(jlex,"JSON lexer unknown number type: '%d'",(int)type);
        }
}

static void nx_json_lexer_STRING(struct NXJSONLexer *jlex, struct NXJSONToken *t)
{
        int len;
        const char * res = nx_lexer_QUOTED_STRING(jlex->lex, &len);
        nx_json_token_fill_string(t, len, res);
}

void nx_json_lexer_next_token(struct NXJSONLexer *jlex, struct NXJSONToken *t)
{
        int c = EOF;
        while ((c = nx_lexer_current_char(jlex->lex)) != EOF) {
                if (nx_json_lexer_is_whitespace(c)) {
                        nx_json_lexer_skip_whitespace(jlex);
                        continue;
                }

                switch (c) {
                case '[': nx_lexer_consume(jlex->lex); nx_json_token_fill(t, NX_JTT_LSBRAC, 0, NULL); return;
                case ']': nx_lexer_consume(jlex->lex); nx_json_token_fill(t, NX_JTT_RSBRAC, 0, NULL); return;
                case '{': nx_lexer_consume(jlex->lex); nx_json_token_fill(t, NX_JTT_LCBRAC, 0, NULL); return;
                case '}': nx_lexer_consume(jlex->lex); nx_json_token_fill(t, NX_JTT_RCBRAC, 0, NULL); return;
                case ',': nx_lexer_consume(jlex->lex); nx_json_token_fill(t, NX_JTT_COMMA, 0, NULL); return;
                case ':': nx_lexer_consume(jlex->lex); nx_json_token_fill(t, NX_JTT_COLON, 0, NULL); return;
                case 't': ; nx_json_lexer_TRUE(jlex, t); return;
                case 'f': ; nx_json_lexer_FALSE(jlex, t); return;
                case 'n': ; nx_json_lexer_NULL(jlex, t); return;
                case '"': ; nx_json_lexer_STRING(jlex, t); return;
                default:
                        if (c == '-' || isdigit(c)) {
                                nx_json_lexer_NUMBER(jlex, t);
                                return;
                        }

                        NX_JSON_LEXER_ERROR(jlex,"Unexpected character while parsing JSON: '%c'",c);
                }
        }

        nx_json_token_fill(t, NX_JTT_EOF, 0, NULL);
}

