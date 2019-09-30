/**
 * @file nx_json_parser.h
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
