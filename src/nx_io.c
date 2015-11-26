/**
 * @file nx_io.c
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
#include "virg/nexus/nx_io.h"

#include <errno.h>

#include "virg/nexus/nx_message.h"

void nx_io_fatal_exit(const char *tag, const char *fmt, ...)
{
        perror(NULL);

        va_list prm;
        va_start(prm, fmt);
        nx_vfatal(tag, fmt, prm);
        va_end(prm);
}

void nx_io_error(const char *tag, const char *fmt, ...)
{
        perror(NULL);

        va_list prm;
        va_start(prm, fmt);
        nx_verror(tag, fmt, prm);
        va_end(prm);
}


FILE *nx_xfopen(const char *path, const char *mode)
{
        FILE *stream = fopen(path, mode);
        if (!stream)
                nx_io_fatal_exit(NX_LOG_TAG, "Error opening file %s",
                                 path);
        else
                return stream;
}

void nx_xfclose(FILE *stream, const char *stream_label)
{
        int e = ferror(stream);

        fclose(stream);

        if (e) {
                 nx_io_fatal_exit(NX_LOG_TAG, "Error detected before closing stream %s",
                                  stream_label);
        }
}

FILE *nx_fopen(const char *path, const char *mode)
{
        FILE *stream = fopen(path, mode);
        if (!stream) {
                nx_io_error(NX_LOG_TAG, "Error opening file %s",
                            path);
        }

        return stream;
}

NXResult nx_fclose(FILE *stream, const char *stream_label)
{
        int e = ferror(stream);

        fclose(stream);

        if (e) {
                nx_io_error(NX_LOG_TAG, "Error detected before closing stream %s",
                            stream_label);
                return NX_FAIL;
        }

        return NX_OK;
}

void nx_xfwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
        if (fwrite(ptr, size, nmemb, stream) != nmemb)
                nx_io_fatal_exit(NX_LOG_TAG, "Error writing %zd items to stream", nmemb);
}

void nx_xfread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
        if (fread(ptr, size, nmemb, stream) != nmemb)
                nx_io_fatal_exit(NX_LOG_TAG, "Error read %zd items from stream", nmemb);
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
