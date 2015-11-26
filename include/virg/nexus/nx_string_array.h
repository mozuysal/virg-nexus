/**
 * @file nx_string_array.h
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
#ifndef VIRG_NEXUS_NX_STRING_ARRAY_H
#define VIRG_NEXUS_NX_STRING_ARRAY_H

#include <stdarg.h>
#include <stdio.h>

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

struct NXStringArray;

struct NXStringArray *nx_string_array_alloc();
struct NXStringArray *nx_string_array_new(int n);
void nx_string_array_free(struct NXStringArray *sa);

int nx_string_array_size(struct NXStringArray *sa);
void nx_string_array_resize(struct NXStringArray *sa, int new_size);
#define nx_string_array_clear(sa) nx_string_array_resize(sa, 0)

char **nx_string_array_ptr(struct NXStringArray *sa);
char *nx_string_array_get(struct NXStringArray *sa, int id);
void nx_string_array_set(struct NXStringArray *sa, int id, const char *s);
void nx_string_array_setf(struct NXStringArray *sa, int id, const char *fmt, ...)  __attribute__((format (printf, 3, 4)));
void nx_string_array_vsetf(struct NXStringArray *sa, int id, const char *fmt, va_list args);

int nx_string_array_append(struct NXStringArray *sa, const char *s);
int nx_string_array_appendf(struct NXStringArray *sa, const char *fmt, ...)  __attribute__((format (printf, 2, 3)));
int nx_string_array_vappendf(struct NXStringArray *sa, const char *fmt, va_list args);

int nx_string_array_find(const struct NXStringArray *sa, const char *s);

int nx_string_array_write(const struct NXStringArray *sa, FILE *stream);
void nx_string_array_xwrite(const struct NXStringArray *sa, FILE *stream);
int nx_string_array_read(struct NXStringArray *sa, FILE *stream);
void nx_string_array_xread(struct NXStringArray *sa, FILE *stream);
struct NXStringArray *nx_string_array_read0(FILE *stream);
struct NXStringArray *nx_string_array_xread0(FILE *stream);

__NX_END_DECL

#endif
