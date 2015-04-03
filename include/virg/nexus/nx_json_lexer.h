/**
 * @file nx_json_lexer.h
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
