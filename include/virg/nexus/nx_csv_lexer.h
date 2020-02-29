/**
 * @file nx_csv_lexer.h
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
#ifndef VIRG_NEXUS_NX_CSV_LEXER_H
#define VIRG_NEXUS_NX_CSV_LEXER_H

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

enum NXCSVTokenType {
        NX_CTT_INVALID = -1,
        NX_CTT_EOF = 0,
        NX_CTT_EOR,
        NX_CTT_COMMA,
        NX_CTT_FIELD,
        NX_CTT_QUOTED_FIELD
};

struct NXCSVToken {
        enum NXCSVTokenType type;
        char *text;
};

void nx_csv_token_free_data(struct NXCSVToken *t);
char *nx_csv_token_to_string(const struct NXCSVToken *t);
const char *nx_csv_token_name(const struct NXCSVToken *t);
const char *nx_csv_token_type_name(enum NXCSVTokenType type);

struct NXCSVLexer;

struct NXCSVLexer *nx_csv_lexer_new(char *csv_text);
void nx_csv_lexer_free(struct NXCSVLexer *clex);
void nx_csv_lexer_next_token(struct NXCSVLexer *clex, struct NXCSVToken *t);

__NX_END_DECL

#endif
