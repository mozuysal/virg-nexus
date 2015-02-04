/**
 * @file nx_json_options.h
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
#ifndef VIRG_NEXUS_NX_JSON_OPTIONS_H
#define VIRG_NEXUS_NX_JSON_OPTIONS_H

#include <stdlib.h>
#include <stdio.h>

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

struct NXJSONOptions;

struct NXJSONOptions *nx_json_options_alloc();
struct NXJSONOptions *nx_json_options_new(char *opt_format, ...);
void nx_json_options_free(struct NXJSONOptions *jopt);

void nx_json_options_add(struct NXJSONOptions *jopt, const char *opt_format, ...);

NXBool      nx_json_options_get_bool  (struct NXJSONOptions *jopt, const char *opt_name);
int         nx_json_options_get_int  (struct NXJSONOptions *jopt, const char *opt_name);
double      nx_json_options_get_double(struct NXJSONOptions *jopt, const char *opt_name);
const char *nx_json_options_get_string(struct NXJSONOptions *jopt, const char *opt_name);

NXBool *nx_json_options_get_bool_array  (struct NXJSONOptions *jopt, const char *opt_name, size_t *length);
int    *nx_json_options_get_int_array  (struct NXJSONOptions *jopt, const char *opt_name, size_t *length);
double *nx_json_options_get_double_array(struct NXJSONOptions *jopt, const char *opt_name, size_t *length);
char  **nx_json_options_get_string_array(struct NXJSONOptions *jopt, const char *opt_name, size_t *length);

void nx_json_options_set_bool  (struct NXJSONOptions *jopt, const char *opt_name, NXBool value);
void nx_json_options_set_int   (struct NXJSONOptions *jopt, const char *opt_name, int value);
void nx_json_options_set_double(struct NXJSONOptions *jopt, const char *opt_name, double value);
void nx_json_options_set_string(struct NXJSONOptions *jopt, const char *opt_name, const char *value);

void nx_json_options_set_bool_array  (struct NXJSONOptions *jopt, const char *opt_name, size_t length, const NXBool *value);
void nx_json_options_set_int_array   (struct NXJSONOptions *jopt, const char *opt_name, size_t length, const int *value);
void nx_json_options_set_double_array(struct NXJSONOptions *jopt, const char *opt_name, size_t length, const double *value);
void nx_json_options_set_string_array(struct NXJSONOptions *jopt, const char *opt_name, size_t length, char **value);

NXBool nx_json_options_is_set(struct NXJSONOptions *jopt, const char *opt_name);

void nx_json_options_dump(const struct NXJSONOptions *jopt, FILE *stream);
void nx_json_options_parse(struct NXJSONOptions *jopt, FILE *stream);

void nx_json_options_save(const struct NXJSONOptions *jopt, const char *filename);
void nx_json_options_load(struct NXJSONOptions *jopt, const char *filename);

__NX_END_DECL

#endif
