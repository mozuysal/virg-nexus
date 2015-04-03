/**
 * @file nx_json_parser.h
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
#ifndef VIRG_NEXUS_NX_JSON_PARSER_H
#define VIRG_NEXUS_NX_JSON_PARSER_H

#include "virg/nexus/nx_config.h"

#include "virg/nexus/nx_json_lexer.h"
#include "virg/nexus/nx_json_tree.h"

__NX_BEGIN_DECL

struct NXJSONParser;

struct NXJSONParser *nx_json_parser_new(struct NXJSONLexer *jlex);
void nx_json_parser_free(struct NXJSONParser *jp);

struct NXJSONNode *nx_json_parser_parse(struct NXJSONParser *jp);

__NX_END_DECL

#endif
