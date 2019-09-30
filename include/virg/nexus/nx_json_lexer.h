/**
 * @file nx_json_lexer.h
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
#ifndef VIRG_NEXUS_NX_JSON_LEXER_H
#define VIRG_NEXUS_NX_JSON_LEXER_H

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

enum NXJSONTokenType {
        NX_JTT_INVALID = -1,
        NX_JTT_EOF = 0,
        NX_JTT_LSBRAC,
        NX_JTT_RSBRAC,
        NX_JTT_LCBRAC,
        NX_JTT_RCBRAC,
        NX_JTT_COMMA,
        NX_JTT_COLON,
        NX_JTT_TRUE,
        NX_JTT_FALSE,
        NX_JTT_NULL,
        NX_JTT_INTEGER,
        NX_JTT_FPNUM,
        NX_JTT_STRING
};

struct NXJSONToken {
        enum NXJSONTokenType type;
        char *text;
};

void nx_json_token_free_data(struct NXJSONToken *t);
char *nx_json_token_to_string(const struct NXJSONToken *t);
const char *nx_json_token_name(const struct NXJSONToken *t);
const char *nx_json_token_type_name(enum NXJSONTokenType type);

struct NXJSONLexer;

struct NXJSONLexer *nx_json_lexer_new(char *json_text);
void nx_json_lexer_free(struct NXJSONLexer *jlex);
void nx_json_lexer_next_token(struct NXJSONLexer *jlex, struct NXJSONToken *t);

__NX_END_DECL

#endif
