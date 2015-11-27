/**
 * @file nx_lexer.c
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
#include "virg/nexus/nx_lexer.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_message.h"

#define LEXER_TAB_WIDTH 8

#define NX_LEXER_ERROR_LINE_COL_INFO "Line %d, Column %d: "
#define NX_LEXER_ERROR(lx,msg,...)                                 \
        do {                                                            \
                fflush(stdout);                                         \
                nx_fatal("NXLexer",NX_LEXER_ERROR_LINE_COL_INFO msg,(lx)->line_no,(lx)->col_no,##__VA_ARGS__); \
        } while (0)


struct NXLexer
{
        char *text;
        int pos;
        int c;

        int line_no;
        int col_no;
};

struct NXLexerState {
        int pos;
        int c;
        int line_no;
        int col_no;
};

static void save_lexer_state(const struct NXLexer *lex, struct NXLexerState *state)
{
        NX_ASSERT_PTR(lex);
        NX_ASSERT_PTR(state);

        state->pos = lex->pos;
        state->c = lex->c;
        state->line_no = lex->line_no;
        state->col_no = lex->col_no;
}

static void load_lexer_state(struct NXLexer *lex, const struct NXLexerState *state)
{
        NX_ASSERT_PTR(lex);
        NX_ASSERT_PTR(state);

        lex->pos = state->pos;
        lex->c = state->c;
        lex->line_no = state->line_no;
        lex->col_no = state->col_no;
}

struct NXLexer *nx_lexer_new(char *text)
{
        NX_ASSERT_PTR(text);

        struct NXLexer *lex = NX_NEW(1, struct NXLexer);
        lex->text = text;
        lex->pos = -1;
        lex->c = EOF;
        lex->line_no = 1;
        lex->col_no = 0;

        nx_lexer_consume(lex);

        return lex;
}

void nx_lexer_free(struct NXLexer *lex)
{
        if (lex != NULL) {
                nx_free(lex);
        }
}

const char *nx_lexer_text(const struct NXLexer *lex)
{
        NX_ASSERT_PTR(lex);
        return lex->text;
}

int nx_lexer_position(const struct NXLexer *lex)
{
        NX_ASSERT_PTR(lex);
        return lex->pos;
}

int nx_lexer_current_char(const struct NXLexer *lex)
{
        NX_ASSERT_PTR(lex);
        return lex->c;
}

int nx_lexer_line_no(const struct NXLexer *lex)
{
        NX_ASSERT_PTR(lex);
        return lex->line_no;
}

int nx_lexer_column_no(const struct NXLexer *lex)
{
        NX_ASSERT_PTR(lex);
        return lex->col_no;
}

int nx_lexer_consume(struct NXLexer *lex)
{
        NX_ASSERT_PTR(lex);

        lex->c = lex->text[++lex->pos];
        if (lex->c == '\0')
                lex->c = EOF;

        if (lex->c == '\n') {
                lex->line_no++;
                lex->col_no = 0;
        } else if (lex->c == '\r') {
                lex->col_no = 0;
        } else if (lex->c == '\t') {
                lex->col_no += LEXER_TAB_WIDTH;
        } else {
                lex->col_no++;
        }

        return lex->c;
}

NXBool nx_lexer_match(struct NXLexer *lex, const char *s)
{
        NX_ASSERT_PTR(lex);
        NX_ASSERT_PTR(s);

        struct NXLexerState start_state;
        save_lexer_state(lex, &start_state);

        int n = strlen(s);
        while (n--) {
                if (lex->c == *s) {
                        nx_lexer_consume(lex);
                } else {
                        load_lexer_state(lex, &start_state);
                        return NX_FALSE;
                }
                ++s;
        }

        return NX_TRUE;
}

const char *nx_lexer_NUMBER(struct NXLexer *lex, int *len, enum NXLexerNumberType *type)
{
        NX_ASSERT_PTR(lex);
        NX_ASSERT_PTR(len);
        NX_ASSERT_PTR(type);

        int start = lex->pos;

        if (lex->c == '-')
                nx_lexer_consume(lex);

        while (isdigit(lex->c))
                nx_lexer_consume(lex);

        int end = lex->pos;
        *len = end-start;

        if (lex->text[start] == '-' && *len == 1)
                NX_LEXER_ERROR(lex,"%c should be followed by at least one digit", '-');
        else if (lex->text[start] == '0' && *len > 1)
                NX_LEXER_ERROR(lex,"Multi-digit numbers can not start with %d", 0);
        else if (*len > 2 && lex->text[start] == '-' && lex->text[start+1] == '0')
                NX_LEXER_ERROR(lex,"Multi-digit numbers can not start with -%d", 0);

        if (lex->c != '.' && lex->c != 'e' && lex->c != 'E') {
                *type = NX_LNT_INTEGER;
                return lex->text + start;
        }

        if (lex->c == '.') {
                nx_lexer_consume(lex);
                if (!isdigit(lex->c))
                        NX_LEXER_ERROR(lex,"Floating-point fraction should have at least one digit%c", '!');
                while (isdigit(lex->c))
                        nx_lexer_consume(lex);
        }

        if (lex->c == 'e' || lex->c == 'E') {
                nx_lexer_consume(lex);
                if (lex->c == '+' || lex->c == '-')
                        nx_lexer_consume(lex);
                if (!isdigit(lex->c))
                        NX_LEXER_ERROR(lex,"Floating-point exponent should have at least one digit%c", '!');
                while (isdigit(lex->c))
                        nx_lexer_consume(lex);
        }

        end = lex->pos;
        *len = end-start;
        *type = NX_LNT_FLOATING_POINT;
        return lex->text + start;
}

const char *nx_lexer_QUOTED_STRING(struct NXLexer *lex, int *len)
{
        NX_ASSERT_PTR(lex);
        NX_ASSERT_PTR(len);

        int start = lex->pos+1; // skip "
        do {
                if (lex->c == '\\') {
                        nx_lexer_consume(lex);
                        switch (lex->c) {
                        case '"':
                        case '\\':
                        case '/':
                        case 'b':
                        case 'f':
                        case 'n':
                        case 'r':
                        case 't':
                                nx_lexer_consume(lex);
                                break;
                        default:
                                NX_LEXER_ERROR(lex,"Unknown control character in string: \\%c", lex->c);
                        }
                } else {
                        nx_lexer_consume(lex);
                }
        } while (lex->c != EOF && lex->c != '"');

        if (lex->c != '"')
                NX_LEXER_ERROR(lex,"Missing closing %c for string", '\"');
        nx_lexer_consume(lex);

        int end = lex->pos-1; // omit "
        *len = end - start;
        return lex->text + start;
}

