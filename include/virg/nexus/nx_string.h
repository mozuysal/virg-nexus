/**
 * @file nx_string.h
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
#ifndef VIRG_NEXUS_NX_STRING_H
#define VIRG_NEXUS_NX_STRING_H

#include <stdarg.h>
#include <stdio.h>

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

char *nx_strdup(const char *s);
void nx_strredup(char ** dest, const char *src);

char *nx_fstr(const char* format, ...) __attribute__((format (printf, 1, 2)));
char *nx_vfstr(const char* format, va_list args);

int nx_strwrite(const char *s, FILE *stream);
int nx_strread(char **s, size_t size, FILE *stream);
char *nx_strread0(FILE *stream);

char *nx_str_from_readable(size_t size, const char *readable);

__NX_END_DECL

#endif
