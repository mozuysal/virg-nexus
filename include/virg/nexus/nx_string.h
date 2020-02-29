/**
 * @file nx_string.h
 *
 * Copyright (C) 2019,2020 Mustafa Ozuysal. All rights reserved.
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
 * You should have received a copy of the GNU General Public License along with
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
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
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

char *nx_strdup(const char *s);
void nx_strredup(char ** dest, const char *src);

char *nx_fstr(const char* format, ...) __attribute__((format (printf, 1, 2)));
char *nx_vfstr(const char* format, va_list args);

int nx_strwrite(const char *s, FILE *stream);
int nx_strread(char **s, size_t size, FILE *stream);
char *nx_strread0(FILE *stream);

char *nx_xstr_from_readable(size_t size, const char *readable);
char *nx_xstr_from_double_quoted(size_t size, const char *dquoted);

size_t nx_memncpy_multi(void *dest, int n_src, const void * const *src,
                        const size_t *l_src, size_t n,
                        int *src_offset, size_t *offset);

char *nx_hash_to_str(int n, uint8_t *hash);

__NX_END_DECL

#endif
