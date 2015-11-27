/**
 * @file nx_lexer.h
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
#ifndef VIRG_NEXUS_NX_LEXER_H
#define VIRG_NEXUS_NX_LEXER_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

enum NXLexerNumberType {
        NX_LNT_INTEGER,
        NX_LNT_FLOATING_POINT
};

struct NXLexer;

struct NXLexer *nx_lexer_new(char *text);
void nx_lexer_free(struct NXLexer *lex);

const char *nx_lexer_text(const struct NXLexer *lex);
int nx_lexer_position(const struct NXLexer *lex);
int nx_lexer_current_char(const struct NXLexer *lex);
int nx_lexer_line_no(const struct NXLexer *lex);
int nx_lexer_column_no(const struct NXLexer *lex);

int    nx_lexer_consume(struct NXLexer *lex);
NXBool nx_lexer_match  (struct NXLexer *lex, const char *s);

const char *nx_lexer_NUMBER       (struct NXLexer *lex, int *len, enum NXLexerNumberType *type);
const char *nx_lexer_QUOTED_STRING(struct NXLexer *lex, int *len);

__NX_END_DECL

#endif
