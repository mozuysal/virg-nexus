/**
 * @file nx_lexer.h
 *
 * Copyright (C) 2019,2020 Mustafa Ozuysal. All rights reserved.
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
