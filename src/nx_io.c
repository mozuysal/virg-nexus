/**
 * @file nx_io.c
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
#include "virg/nexus/nx_io.h"

#include <errno.h>

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_math.h"

FILE *nx_xfopen(const char *path, const char *mode)
{
        NX_ASSERT_PTR(path);
        NX_ASSERT_PTR(mode);

        FILE *stream = fopen(path, mode);
        if (!stream)
                NX_IO_FATAL(NX_LOG_TAG, "Error opening file %s",
                                 path);
        else
                return stream;
}

void nx_xfclose(FILE *stream, const char *stream_label)
{
        NX_ASSERT_PTR(stream);

        int e = ferror(stream);

        fclose(stream);

        if (e) {
                 NX_IO_FATAL(NX_LOG_TAG, "Error detected before closing stream %s",
                                  stream_label);
        }
}

FILE *nx_fopen(const char *path, const char *mode)
{
        NX_ASSERT_PTR(path);
        NX_ASSERT_PTR(mode);

        FILE *stream = fopen(path, mode);
        if (!stream) {
                NX_IO_ERROR(NX_LOG_TAG, "Error opening file %s",
                            path);
        }

        return stream;
}

NXResult nx_fclose(FILE *stream, const char *stream_label)
{
        NX_ASSERT_PTR(stream);

        int e = ferror(stream);

        fclose(stream);

        if (e) {
                NX_IO_ERROR(NX_LOG_TAG, "Error detected before closing stream %s",
                            stream_label);
                return NX_FAIL;
        }

        return NX_OK;
}

void nx_xfwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
        if (fwrite(ptr, size, nmemb, stream) != nmemb)
                NX_IO_FATAL(NX_LOG_TAG, "Error writing %zd items to stream", nmemb);
}

void nx_xfread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
        if (fread(ptr, size, nmemb, stream) != nmemb)
                NX_IO_FATAL(NX_LOG_TAG, "Error read %zd items from stream", nmemb);
}

int nx_fputs_readable(const char *s, FILE *stream)
{
        if (s == NULL) {
                if (4 == fprintf(stream, "null"))
                        return 0;
                else
                        return EOF;
        }

        if (EOF == fputc('\"', stream))
                return EOF;
        while (*s != '\0') {
                switch (*s) {
                case '\n': if (EOF == fputs("\\n", stream)) return EOF; break;
                case '\r': if (EOF == fputs("\\r", stream)) return EOF; break;
                case '\t': if (EOF == fputs("\\t", stream)) return EOF; break;
                case '\b': if (EOF == fputs("\\b", stream)) return EOF; break;
                case '\f': if (EOF == fputs("\\f", stream)) return EOF; break;
                case '\"': if (EOF == fputs("\\\"", stream)) return EOF; break;
                case '\\': if (EOF == fputs("\\\\", stream)) return EOF; break;
                default:
                        if (EOF == fputc(*s, stream))
                                return EOF;
                }

                ++s;
        }
        if (EOF == fputc('\"', stream))
                return EOF;

        return 0;
}

int nx_fputs_double_quoted(const char *s, FILE *stream)
{
        if (s == NULL)
                return 0;

        if (EOF == fputc('\"', stream)) {
                return EOF;
        }
        while (*s != '\0') {
                if (*s == '\"') {
                        if (EOF == fputs("\"\"", stream)) {
                                return EOF;
                        }
                } else {
                        if (EOF == fputc(*s, stream)) {
                                return EOF;
                        }
                }
                ++s;
        }
        if (EOF == fputc('\"', stream)) {
                return EOF;
        }

        return 0;
}

FILE *nx_xtmpfile(void)
{
        FILE *stream = tmpfile();
        if (!stream)
                NX_IO_FATAL(NX_LOG_TAG, "Error creating and opening temporary file");

        return stream;
}

#define NX_EXPAND_IF_NECESSARY_AND_APPEND_CHAR(line,n,idx,c) \
        do {                                                            \
                if (idx >= *n) {                                        \
                        int new_sz = nx_max_i(4, (int)(*n * 1.6f));     \
                        *line = nx_xrealloc(*line, new_sz * sizeof(char)); \
                        *n = new_sz;                                    \
                }                                                       \
                (*line)[idx++] = c;                                     \
        } while (0)


ssize_t nx_getline(char **line, size_t *n, FILE *stream)
{
        NX_ASSERT_PTR(line);
        NX_ASSERT_PTR(n);
        NX_ASSERT_PTR(stream);

        int c;
        size_t idx = 0;
        while ((c = fgetc(stream)) != '\n' && c != EOF) {
                NX_EXPAND_IF_NECESSARY_AND_APPEND_CHAR(line,n,idx,c);
        }

        if (c == '\n') {
                NX_EXPAND_IF_NECESSARY_AND_APPEND_CHAR(line,n,idx,c);
        }

        NX_EXPAND_IF_NECESSARY_AND_APPEND_CHAR(line,n,idx,0);

        if (idx == 0) {
                return -1;
        } else {
                return idx - 1;
        }
}
