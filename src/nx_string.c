/**
 * @file nx_string.c
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
#include "virg/nexus/nx_string.h"

#include <string.h>
#include <stdio.h>

#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_message.h"

#define NX_FSTR_INITIAL_LENGTH 64

char *nx_strdup(const char *s)
{
        size_t lcpy = strlen(s)+1;
        char *cpy = NX_NEW_C(lcpy);

        strcpy(cpy, s);

        return cpy;
}

void nx_strredup(char **dest, const char *src)
{
        nx_free(*dest);

        if (src == NULL)
                *dest = NULL;
        else
                *dest = nx_strdup(src);
}

char *nx_fstr(const char* format, ...)
{
        va_list arg_list;
        va_start(arg_list, format);
        char *s = nx_vfstr(format, arg_list);
        va_end(arg_list);

        return s;
}

char *nx_vfstr(const char* format, va_list args)
{
        size_t ls = NX_FSTR_INITIAL_LENGTH+1;
        char *s = NX_NEW_C(ls);

        int n = 0;
        while (NX_TRUE) {
                va_list args_cpy;
                va_copy(args_cpy, args);
                n = vsnprintf(s, ls, format, args_cpy);
                va_end(args_cpy);

                if (n < 0)
                        nx_fatal(NX_LOG_TAG, "Error making string from format %s",
                                 format);

                if (n >= ls) {
                        ls = n+1;
                        s = nx_frealloc(s, ls);
                } else {
                        return s;
                }
        };
}
