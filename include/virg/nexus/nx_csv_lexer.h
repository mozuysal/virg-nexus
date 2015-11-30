/**
 * @file nx_csv_lexer.h
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
