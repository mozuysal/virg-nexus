/**
 * @file nx_io.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Izmir Institute of Technology. All rights reserved.
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

__NX_BEGIN_DECL

__NX_NO_RETURN void nx_io_fatal_exit(const char *tag, const char *fmt, ...) __attribute__((format (printf, 2, 3)));

void nx_io_error(const char *tag, const char *fmt, ...) __attribute__((format (printf, 2, 3)));

FILE *nx_xfopen(const char *path, const char *mode);

void nx_xfclose(FILE *stream, const char *stream_label);

FILE *nx_fopen(const char *path, const char *mode);

NXResult nx_fclose(FILE *stream, const char *stream_label);

__NX_END_DECL

#endif
