/**
 * @file nx_csv_parser.h
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
