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

int nx_strwrite(const char *s, FILE *stream)
{
        size_t ls = strlen(s);
        if (fwrite(&ls, sizeof(ls), 1, stream) != 1)
                return 1;

        if (fwrite(s, sizeof(*s), ls, stream) != ls)
                return 1;

        return 0;
}

int nx_strread(char **s, size_t size, FILE *stream)
{
        size_t ls = 0;
        if (fread(&ls, sizeof(ls), 1, stream) != 1)
                return 1;

        if (ls >= size)
                *s = nx_frealloc(*s, (ls+1)*sizeof(**s));

        size_t n_read = fread(*s, sizeof(**s), ls, stream);
        (*s)[ls] = '\0';

        if (n_read != ls)
                return 1;

        return 0;
}

char *nx_strread0(FILE *stream)
{
        char *s = NULL;
        if (nx_strread(&s, 0, stream) != 0) {
                if (s != NULL)
                        nx_free(s);
                return NULL;
        }

        return s;
}

char *nx_xstr_from_readable(size_t size, const char *readable)
{
        if (readable == NULL)
                return NULL;

        char *s = NX_NEW_C(size+1);
        char *p = s;
        while (size--) {
                if (*readable == '\\') {
                        if (size == 0)
                                nx_fatal(NX_LOG_TAG, "Missing control character after \\ in readable string!");

                        readable++;
                        size--;
                        switch (*readable) {
                        case 'r': *(p++) = '\r'; break;
                        case 't': *(p++) = '\t'; break;
                        case 'n': *(p++) = '\n'; break;
                        case '\\': *(p++) = '\\'; break;
                        case 'f': *(p++) = '\f'; break;
                        case 'b': *(p++) = '\b'; break;
                        case '"': *(p++) = '"'; break;
                        default:
                                nx_fatal(NX_LOG_TAG, "Unexpected control character '%c' in readable string!", *readable);
                        }
                        readable++;
                } else {
                        *(p++) = *(readable++);
                }
        }
        *p = '\0';
        return s;
}

char *nx_xstr_from_double_quoted(size_t size, const char *dquoted)
{
        if (dquoted == NULL)
                return NULL;

        char *s = NX_NEW_C(size+1);
        char *p = s;
        while (size--) {
                if (*dquoted == '"') {
                        if (size == 0)
                                nx_fatal(NX_LOG_TAG, "Missing double quote in double quoted string!");

                        dquoted++;
                        size--;
                        if (*dquoted == '"')
                                *(p++) = '"';
                        else
                                nx_fatal(NX_LOG_TAG, "Expected \" found '%c' in double quoted string!", *dquoted);
                        dquoted++;
                } else {
                        *(p++) = *(dquoted++);
                }
        }
        *p = '\0';
        return s;
}
