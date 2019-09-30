/**
 * @file nx_io.c
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
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/nx_io.h"

#include <errno.h>

FILE *nx_xfopen(const char *path, const char *mode)
{
        FILE *stream = fopen(path, mode);
        if (!stream)
                NX_IO_FATAL(NX_LOG_TAG, "Error opening file %s",
                                 path);
        else
                return stream;
}

void nx_xfclose(FILE *stream, const char *stream_label)
{
        int e = ferror(stream);

        fclose(stream);

        if (e) {
                 NX_IO_FATAL(NX_LOG_TAG, "Error detected before closing stream %s",
                                  stream_label);
        }
}

FILE *nx_fopen(const char *path, const char *mode)
{
        FILE *stream = fopen(path, mode);
        if (!stream) {
                NX_IO_ERROR(NX_LOG_TAG, "Error opening file %s",
                            path);
        }

        return stream;
}

NXResult nx_fclose(FILE *stream, const char *stream_label)
{
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

void nx_fputs_readable(const char *s, FILE *stream)
{
        if (s == NULL) {
                fprintf(stream, "null");
                return;
        }

        fputc('\"', stream);
        while (*s != '\0') {
                switch (*s) {
                case '\n': fputs("\\n", stream); break;
                case '\r': fputs("\\r", stream); break;
                case '\t': fputs("\\t", stream); break;
                case '\b': fputs("\\b", stream); break;
                case '\f': fputs("\\f", stream); break;
                case '\"': fputs("\\\"", stream); break;
                case '\\': fputs("\\\\", stream); break;
                default:
                        fputc(*s, stream);
                }

                ++s;
        }
        fputc('\"', stream);
}

void nx_fputs_double_quoted(const char *s, FILE *stream)
{
        if (s == NULL)
                return;

        fputc('\"', stream);
        while (*s != '\0') {
                if (*s == '\"')
                        fputs("\"\"", stream);
                else
                        fputc(*s, stream);
                ++s;
        }
        fputc('\"', stream);
}

FILE *nx_xtmpfile(void)
{
        FILE *stream = tmpfile();
        if (!stream)
                NX_IO_FATAL(NX_LOG_TAG, "Error creating and opening temporary file");

        return stream;
}
