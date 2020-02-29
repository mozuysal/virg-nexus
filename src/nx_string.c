/**
 * @file nx_string.c
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
#include "virg/nexus/nx_string.h"

#include <string.h>
#include <stdio.h>

#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_log.h"

#define NX_FSTR_INITIAL_LENGTH 64

char *nx_strdup(const char *s)
{
        NX_ASSERT_PTR(s);

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
        NX_ASSERT_PTR(format);

        va_list arg_list;
        va_start(arg_list, format);
        char *s = nx_vfstr(format, arg_list);
        va_end(arg_list);

        return s;
}

char *nx_vfstr(const char* format, va_list args)
{
        NX_ASSERT_PTR(format);

        size_t ls = NX_FSTR_INITIAL_LENGTH+1;
        char *s = NX_NEW_C(ls);

        int n = 0;
        while (NX_TRUE) {
                va_list args_cpy;
                va_copy(args_cpy, args);
                n = vsnprintf(s, ls, format, args_cpy);
                va_end(args_cpy);

                if (n < 0)
                        NX_FATAL(NX_LOG_TAG, "Error making string from format %s",
                                 format);

                if ((size_t)n >= ls) {
                        ls = n+1;
                        s = nx_xrealloc(s, ls);
                } else {
                        return s;
                }
        };
}

int nx_strwrite(const char *s, FILE *stream)
{
        NX_ASSERT_PTR(s);
        NX_ASSERT_PTR(stream);

        size_t ls = strlen(s);
        if (fwrite(&ls, sizeof(ls), 1, stream) != 1)
                return 1;

        if (fwrite(s, sizeof(*s), ls, stream) != ls)
                return 1;

        return 0;
}

int nx_strread(char **s, size_t size, FILE *stream)
{
        NX_ASSERT_PTR(s);
        NX_ASSERT_PTR(stream);

        size_t ls = 0;
        if (fread(&ls, sizeof(ls), 1, stream) != 1)
                return 1;

        if (ls >= size)
                *s = nx_xrealloc(*s, (ls+1)*sizeof(**s));

        size_t n_read = fread(*s, sizeof(**s), ls, stream);
        (*s)[ls] = '\0';

        if (n_read != ls)
                return 1;

        return 0;
}

char *nx_strread0(FILE *stream)
{
        NX_ASSERT_PTR(stream);

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
                                NX_FATAL(NX_LOG_TAG, "Missing control character after \\ in readable string!");

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
                                NX_FATAL(NX_LOG_TAG, "Unexpected control character '%c' in readable string!", *readable);
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
                                NX_FATAL(NX_LOG_TAG, "Missing double quote in double quoted string!");

                        dquoted++;
                        size--;
                        if (*dquoted == '"')
                                *(p++) = '"';
                        else
                                NX_FATAL(NX_LOG_TAG, "Expected \" found '%c' in double quoted string!", *dquoted);
                        dquoted++;
                } else {
                        *(p++) = *(dquoted++);
                }
        }
        *p = '\0';
        return s;
}

size_t nx_memncpy_multi(void *dest, int n_src, const void * const *src,
                        const size_t *l_src, size_t n,
                        int *src_offset, size_t *offset)
{
        NX_ASSERT_PTR(dest);
        NX_ASSERT(n_src > 0);
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(l_src);
        NX_ASSERT_PTR(src_offset);
        NX_ASSERT_PTR(offset);

        if (*offset > l_src[*src_offset]) {
                *offset = 0;
                *src_offset += 1;
        }

        size_t n_copied = 0;
        while (n > 0 && *src_offset < n_src) {
                size_t l_i = l_src[*src_offset];
                size_t l_left = l_i - *offset;
                if (l_left > n) {
                        memcpy(dest, src[*src_offset] + *offset, n);
                        n_copied += n;
                        *offset += n;
                        dest += n;
                        n = 0;
                } else {
                        memcpy(dest, src[*src_offset] + *offset, l_left);
                        n -= l_left;
                        n_copied += l_left;
                        *offset = 0;
                        *src_offset += 1;
                        dest += l_left;
                }
        }

        return n_copied;
}

char *nx_hash_to_str(int n, uint8_t *hash)
{
        NX_ASSERT(n >= 0);
        NX_ASSERT_PTR(hash);

        int ls = 2*n + 1;
        char *s = NX_NEW_C(ls);
        for (int i = 0; i < n; ++i)
                sprintf(s + 2*i, "%02x", hash[i]);
        s[ls-1] = '\0';

        return s;
}
