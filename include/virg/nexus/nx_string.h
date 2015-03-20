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

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

char *nx_strdup(const char *s);
void nx_strredup(char ** dest, const char *src);

char *nx_fstr(const char* format, ...) __attribute__((format (printf, 1, 2)));
char *nx_vfstr(const char* format, va_list args);

__NX_END_DECL

#endif
