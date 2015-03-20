/**
 * @file nx_options.h
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

void nx_options_add(struct NXOptions *opt, const char *opt_format, ...);
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
