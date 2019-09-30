/**
 * @file nx_options.h
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
#ifndef VIRG_NEXUS_NX_OPTIONS_H
#define VIRG_NEXUS_NX_OPTIONS_H

#include <stdlib.h>
#include <stdio.h>

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

struct NXOptions;

struct NXOptions *nx_options_alloc();
struct NXOptions *nx_options_new(const char *opt_format, ...);
void nx_options_free(struct NXOptions *opt);

/**
 * Add switches to the options. There are five possible types. Use 'b' for
 * boolean, 'i' for integer, 'd' for double, 's' for string options and 'r' for
 * the special rest option. 'bids' takes three arguments of the following types:
 *   char*: switches, '-<char>' for short form, '--<str>' for long form or '-<char>|--<str>' for both
 *   char*: help string
 *   NXBool|int|double|char*: default value
 *
 * rest takes only a char* that is the name of the help variable and gathers all unprocessed options.
 *
 * @param opt        variable holding options
 * @param opt_format format string composed as described above
 */
void nx_options_add(struct NXOptions *opt, const char *opt_format, ...);

/**
 * Adds the switches '-h|--help' that displays the usage message to options.
 *
 * @param opt variable holding options
 */
void nx_options_add_help(struct NXOptions *opt);

void nx_options_set_from_args(struct NXOptions *opt, int argc, char **argv);

NXBool      nx_options_get_bool  (const struct NXOptions *opt, const char *opt_name);
int         nx_options_get_int   (const struct NXOptions *opt, const char *opt_name);
double      nx_options_get_double(const struct NXOptions *opt, const char *opt_name);
const char *nx_options_get_string(const struct NXOptions *opt, const char *opt_name);
char **     nx_options_get_rest  (const struct NXOptions *opt);

NXBool nx_options_is_set(const struct NXOptions *opt, const char *opt_name);

void nx_options_print_values(const struct NXOptions *opt, FILE* stream);

void nx_options_print_usage(const struct NXOptions *opt, FILE* stream);
void nx_options_set_usage_header(struct NXOptions *opt, const char *header);
void nx_options_set_usage_footer(struct NXOptions *opt, const char *footer);

__NX_END_DECL

#endif
