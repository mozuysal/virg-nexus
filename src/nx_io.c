/**
 * @file nx_io.c
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

FILE *nx_xfopen(const char *path, const char *mode)
{
        FILE *stream = fopen(path, mode);
        if (!stream)
                nx_io_fatal_exit("nx_xfopen", "Error opening file %s",
                                 path);
        else
                return stream;
}

void nx_xfclose(FILE *stream, const char *stream_label)
{
        int e = ferror(stream);

        fclose(stream);

        if (e) {
                 nx_io_fatal_exit("nx_xfclose", "Error detected before closing stream %s",
                                  stream_label);
        }
}
