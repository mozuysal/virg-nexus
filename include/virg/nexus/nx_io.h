/**
 * @file nx_io.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Mustafa Ozuysal. All rights reserved.
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

void nx_fputs_readable(const char *s, FILE *stream);
void nx_fputs_double_quoted(const char *s, FILE *stream);

__NX_END_DECL

#endif
