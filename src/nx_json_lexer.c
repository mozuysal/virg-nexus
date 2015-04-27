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

#define JSON_TAB_WIDTH 8

#define NX_JSON_LEXER_ERROR_LINE_COL_INFO "Line %d, Column %d: "
#define NX_JSON_LEXER_ERROR(lx,msg,...)                                 \
        do {                                                            \
                fflush(stdout);                                         \
                nx_fatal("NXJSONLexer",NX_JSON_LEXER_ERROR_LINE_COL_INFO msg,(lx)->line_no,(lx)->col_no,##__VA_ARGS__); \
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

        if (text_ptr != NULL) {
                t->text = NX_NEW_C(text_length+1);
                char *p = t->text;
                while (text_length--) {
                        if (*text_ptr == '\\') {
                                text_ptr++;
                                text_length--;
                                switch (*text_ptr) {
                                case 'r': *(p++) = '\r'; break;
                                case 't': *(p++) = '\t'; break;
                                case 'n': *(p++) = '\n'; break;
                                case '\\': *(p++) = '\\'; break;
                                case 'f': *(p++) = '\f'; break;
                                case 'b': *(p++) = '\b'; break;
                                case '"': *(p++) = '"'; break;
                                }
                                text_ptr++;
                        } else {
                                *(p++) = *(text_ptr++);
                        }
                }
                *p = '\0';
        }
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
        char *s;
        int p;
        int c;

        int line_no;
        int col_no;
};

static void nx_json_lexer_consume(struct NXJSONLexer *jlex)
{
        jlex->c = jlex->s[++jlex->p];
        if (jlex->c == '\0')
                jlex->c = EOF;

        if (jlex->c == '\n') {
                jlex->line_no++;
                jlex->col_no = 0;
        } else if (jlex->c == '\r') {
                jlex->col_no = 0;
        } else if (jlex->c == '\t') {
                jlex->col_no += JSON_TAB_WIDTH;
        } else {
                jlex->col_no++;
        }
}

struct NXJSONLexer *nx_json_lexer_new(char *json_text)
{
        struct NXJSONLexer *jlex = NX_NEW(1, struct NXJSONLexer);
        jlex->s = json_text;
        jlex->p = -1;
        jlex->c = EOF;
        jlex->line_no = 1;
        jlex->col_no = 0;

        nx_json_lexer_consume(jlex);

        return jlex;
}

void nx_json_lexer_free(struct NXJSONLexer *jlex)
{
        if (jlex != NULL) {
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
        while (nx_json_lexer_is_whitespace(jlex->c))
                nx_json_lexer_consume(jlex);
}

static void nx_json_lexer_match(struct NXJSONLexer *jlex, const char *s)
{
        int n = strlen(s);
        while (n--) {
                if (jlex->c == *s)
                        nx_json_lexer_consume(jlex);
                else
                        NX_JSON_LEXER_ERROR(jlex,"Expecting '%c', found '%c'!",*s,jlex->c);
                ++s;
        }
}

static void nx_json_lexer_TRUE(struct NXJSONLexer *jlex, struct NXJSONToken *t)
{
        nx_json_lexer_consume(jlex);
        nx_json_lexer_match(jlex, "rue");
        nx_json_token_fill(t, NX_JTT_TRUE, 0, NULL);
}

static void nx_json_lexer_FALSE(struct NXJSONLexer *jlex, struct NXJSONToken *t)
{
        nx_json_lexer_consume(jlex);
        nx_json_lexer_match(jlex, "alse");
        nx_json_token_fill(t, NX_JTT_FALSE, 0, NULL);
}

static void nx_json_lexer_NULL(struct NXJSONLexer *jlex, struct NXJSONToken *t)
{
        nx_json_lexer_consume(jlex);
        nx_json_lexer_match(jlex, "ull");
        nx_json_token_fill(t, NX_JTT_NULL, 0, NULL);
}

static void nx_json_lexer_NUMBER(struct NXJSONLexer *jlex, struct NXJSONToken *t)
{
        int start = jlex->p;

        if (jlex->c == '-')
                nx_json_lexer_consume(jlex);

        while (isdigit(jlex->c))
                nx_json_lexer_consume(jlex);

        int end = jlex->p;
        int len = end-start;

        if (jlex->s[start] == '-' && len == 1)
                NX_JSON_LEXER_ERROR(jlex,"%c should be followed by at least one digit", 'c');
        else if (jlex->s[start] == '0' && len > 1)
                NX_JSON_LEXER_ERROR(jlex,"Multi-digit numbers can not start with %d", 0);
        else if (len > 2 && jlex->s[start] == '-' && jlex->s[start+1] == '0')
                NX_JSON_LEXER_ERROR(jlex,"Multi-digit numbers can not start with -%d", 0);

        if (jlex->c != '.' && jlex->c != 'e' && jlex->c != 'E') {
                nx_json_token_fill(t, NX_JTT_INTEGER, len, jlex->s + start);
                return;
        }

        if (jlex->c == '.') {
                nx_json_lexer_consume(jlex);
                if (!isdigit(jlex->c))
                        NX_JSON_LEXER_ERROR(jlex,"Floating-point fraction should have at least one digit%c", '!');
                while (isdigit(jlex->c))
                        nx_json_lexer_consume(jlex);
        }

        if (jlex->c == 'e' || jlex->c == 'E') {
                nx_json_lexer_consume(jlex);
                if (jlex->c == '+' || jlex->c == '-')
                        nx_json_lexer_consume(jlex);
                if (!isdigit(jlex->c))
                        NX_JSON_LEXER_ERROR(jlex,"Floating-point exponent should have at least one digit%c", '!');
                while (isdigit(jlex->c))
                        nx_json_lexer_consume(jlex);
        }

        end = jlex->p;
        len = end-start;
        nx_json_token_fill(t, NX_JTT_FPNUM, len, jlex->s + start);
}

static void nx_json_lexer_STRING(struct NXJSONLexer *jlex, struct NXJSONToken *t)
{
        int start = jlex->p+1; // skip "
        do {
                if (jlex->c == '\\') {
                        nx_json_lexer_consume(jlex);
                        switch (jlex->c) {
                        case '"':
                        case '\\':
                        case '/':
                        case 'b':
                        case 'f':
                        case 'n':
                        case 'r':
                        case 't':
                                nx_json_lexer_consume(jlex);
                                break;
                        default:
                                NX_JSON_LEXER_ERROR(jlex,"Unknown control character in string: \\%c", jlex->c);
                        }
                } else {
                        nx_json_lexer_consume(jlex);
                }
        } while (jlex->c != EOF && jlex->c != '"');

        if (jlex->c != '"')
                NX_JSON_LEXER_ERROR(jlex,"Missing closing %c for string", '\"');
        nx_json_lexer_consume(jlex);

        int end = jlex->p-1; // omit "
        int len = end - start;
        nx_json_token_fill_string(t, len, jlex->s + start);
}

void nx_json_lexer_next_token(struct NXJSONLexer *jlex, struct NXJSONToken *t)
{
        while (jlex->c != EOF) {
                if (nx_json_lexer_is_whitespace(jlex->c)) {
                        nx_json_lexer_skip_whitespace(jlex);
                        continue;
                }

                switch (jlex->c) {
                case '[': nx_json_lexer_consume(jlex); nx_json_token_fill(t, NX_JTT_LSBRAC, 0, NULL); return;
                case ']': nx_json_lexer_consume(jlex); nx_json_token_fill(t, NX_JTT_RSBRAC, 0, NULL); return;
                case '{': nx_json_lexer_consume(jlex); nx_json_token_fill(t, NX_JTT_LCBRAC, 0, NULL); return;
                case '}': nx_json_lexer_consume(jlex); nx_json_token_fill(t, NX_JTT_RCBRAC, 0, NULL); return;
                case ',': nx_json_lexer_consume(jlex); nx_json_token_fill(t, NX_JTT_COMMA, 0, NULL); return;
                case ':': nx_json_lexer_consume(jlex); nx_json_token_fill(t, NX_JTT_COLON, 0, NULL); return;
                case 't': ; nx_json_lexer_TRUE(jlex, t); return;
                case 'f': ; nx_json_lexer_FALSE(jlex, t); return;
                case 'n': ; nx_json_lexer_NULL(jlex, t); return;
                case '"': ; nx_json_lexer_STRING(jlex, t); return;
                default:
                        if (jlex->c == '-' || isdigit(jlex->c)) {
                                nx_json_lexer_NUMBER(jlex, t);
                                return;
                        }

                        NX_JSON_LEXER_ERROR(jlex,"Unexpected character while parsing JSON: '%c'",jlex->c);
                }
        }

        nx_json_token_fill(t, NX_JTT_EOF, 0, NULL);
}

