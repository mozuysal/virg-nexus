/**
 * @file nx_csv_parser.h
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
#ifndef VIRG_NEXUS_NX_CSV_PARSER_H
#define VIRG_NEXUS_NX_CSV_PARSER_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_csv_lexer.h"
#include "virg/nexus/nx_data_frame.h"

__NX_BEGIN_DECL

struct NXCSVParser;

struct NXCSVParser *nx_csv_parser_new(struct NXCSVLexer *clex);
void nx_csv_parser_free(struct NXCSVParser *cp);

struct NXDataFrame *nx_csv_parser_parse(struct NXCSVParser *cp);

__NX_END_DECL

#endif
