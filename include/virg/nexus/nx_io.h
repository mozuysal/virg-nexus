/**
 * @file nx_io.h
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
 * You should have received a copy of the GNU General Public License along with
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_IO_H
#define VIRG_NEXUS_NX_IO_H

#include "virg/nexus/nx_config.h"

#include <stdio.h>

#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_log.h"

__NX_BEGIN_DECL

#define NX_IO_FATAL(tag,fmt,...)                        \
        do {                                            \
                perror(NULL);                           \
                NX_FATAL(tag,fmt,##__VA_ARGS__);        \
        } while(0)

#define NX_IO_ERROR(tag,fmt,...)                   \
        do {                                       \
                perror(NULL);                      \
                NX_ERROR(tag, fmt, ##__VA_ARGS__); \
        } while(0)

FILE *nx_xfopen(const char *path, const char *mode);
void nx_xfclose(FILE *stream, const char *stream_label);

FILE *nx_fopen(const char *path, const char *mode);
NXResult nx_fclose(FILE *stream, const char *stream_label);

FILE *nx_xtmpfile(void);

void nx_xfwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
void nx_xfread(void *ptr, size_t size, size_t nmemb, FILE *stream);

int nx_fputs_readable(const char *s, FILE *stream);
int nx_fputs_double_quoted(const char *s, FILE *stream);

ssize_t nx_getline(char **line, size_t *n, FILE *stream);

__NX_END_DECL

#endif
