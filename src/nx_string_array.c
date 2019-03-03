/**
 * @file nx_string_array.c
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
#include "virg/nexus/nx_string_array.h"

#include <stdio.h>
#include <string.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_string.h"

struct NXStringArray
{
        char **elems;
        int size;
        int capacity;
};

struct NXStringArray *nx_string_array_alloc()
{
        struct NXStringArray *sa = NX_NEW(1, struct NXStringArray);
        sa->capacity = 2;
        sa->elems = NX_NEW(sa->capacity, char *);
        sa->size = 0;
        return sa;
}

struct NXStringArray *nx_string_array_new(int n)
{
        struct NXStringArray *sa = nx_string_array_alloc();
        nx_string_array_resize(sa, n);
        return sa;
}

void nx_string_array_free(struct NXStringArray *sa)
{
        if (sa) {
                for (int i = 0; i < sa->size; ++i)
                        nx_free(sa->elems[i]);
                nx_free(sa->elems);
                nx_free(sa);
        }
}

int nx_string_array_size(struct NXStringArray *sa)
{
        NX_ASSERT_PTR(sa);

        return sa->size;
}

void nx_string_array_resize(struct NXStringArray *sa, int new_size)
{
        NX_ASSERT_PTR(sa);
        NX_ASSERT(new_size >= 0);

        if (new_size < sa->size) {
                for (int i = new_size; i < sa->size; ++i) {
                        nx_free(sa->elems[i]);
                        sa->elems[i] = NULL;
                }
        } else {
                NX_ENSURE_CAPACITY(sa->elems, sa->capacity, new_size);
                for (int i = sa->size; i < new_size; ++i)
                        sa->elems[i] = NULL;
        }

        sa->size = new_size;
}

char **nx_string_array_ptr(struct NXStringArray *sa)
{
        NX_ASSERT_PTR(sa);

        return sa->elems;
}

char *nx_string_array_get(struct NXStringArray *sa, int id)
{
        NX_ASSERT_PTR(sa);
        NX_ASSERT(id >= 0 && id < sa->size);

        return sa->elems[id];
}

void nx_string_array_set(struct NXStringArray *sa, int id, const char *s)
{
        NX_ASSERT_PTR(sa);
        NX_ASSERT(id >= 0 && id < sa->size);

        nx_strredup(sa->elems + id, s);
}

void nx_string_array_setf(struct NXStringArray *sa, int id, const char *fmt, ...)
{
        va_list args;
        va_start(args, fmt);
        nx_string_array_vsetf(sa, id, fmt, args);
        va_end(args);
}

void nx_string_array_vsetf(struct NXStringArray *sa, int id, const char *fmt, va_list args)
{
        NX_ASSERT_PTR(sa);
        NX_ASSERT(id >= 0 && id < sa->size);
        NX_ASSERT_PTR(fmt);

        nx_free(sa->elems[id]);
        sa->elems[id] = nx_vfstr(fmt, args);
}

int nx_string_array_append(struct NXStringArray *sa, const char *s)
{
        NX_ASSERT_PTR(sa);

        int n = sa->size;
        nx_string_array_resize(sa, n+1);
        nx_string_array_set(sa, n, s);
        return n;
}

int nx_string_array_appendf(struct NXStringArray *sa, const char *fmt, ...)
{
        va_list args;
        va_start(args, fmt);
        int n = nx_string_array_vappendf(sa, fmt, args);
        va_end(args);
        return n;
}

int nx_string_array_vappendf(struct NXStringArray *sa, const char *fmt, va_list args)
{
        NX_ASSERT_PTR(sa);
        NX_ASSERT_PTR(fmt);

        int n = sa->size;
        nx_string_array_resize(sa, n+1);
        nx_string_array_vsetf(sa, n, fmt, args);
        return n;
}

int nx_string_array_find(const struct NXStringArray *sa, const char *s)
{
        NX_ASSERT_PTR(sa);
        NX_ASSERT_PTR(s);

        for (int i = 0; i < sa->size; ++i)
                if (strcmp(s, sa->elems[i]) == 0)
                        return i;

        return -1;
}

int nx_string_array_write(const struct NXStringArray *sa, FILE *stream)
{
        NX_ASSERT_PTR(sa);

        if (fwrite(&sa->size, sizeof(sa->size), 1, stream) != 1) {
                NX_ERROR(NX_LOG_TAG, "Error writing string array length to stream");
                return -1;
        }

        for (int i = 0; i < sa->size; ++i)
                if (nx_strwrite(sa->elems[i], stream) != 0) {
                        NX_ERROR(NX_LOG_TAG, "Error writing string array element %d to stream: '%s'",
                                 i, sa->elems[i]);
                        return -1;
                }

        return 0;
}

void nx_string_array_xwrite(const struct NXStringArray *sa, FILE *stream)
{
        NX_ASSERT_PTR(sa);

        if (nx_string_array_write(sa, stream) != 0)
                NX_FATAL(NX_LOG_TAG, "Error writing string array to stream");
}

int nx_string_array_read(struct NXStringArray *sa, FILE *stream)
{
        NX_ASSERT_PTR(sa);

        int size = 0;
        if (fread(&size, sizeof(size), 1, stream) != 1) {
                NX_ERROR(NX_LOG_TAG, "Error writing string array length to stream");
                return -1;
        }

        nx_string_array_resize(sa, size);
        for (int i = 0; i < sa->size; ++i)
                if (nx_strread(&sa->elems[i], 0, stream) != 0) {
                        NX_ERROR(NX_LOG_TAG, "Error reading string array element %d from stream", i);
                        return -1;
                }

        return 0;
}

void nx_string_array_xread(struct NXStringArray *sa, FILE *stream)
{
        NX_ASSERT_PTR(sa);

        if (nx_string_array_read(sa, stream) != 0)
                NX_FATAL(NX_LOG_TAG, "Error reading string array from stream");
}

struct NXStringArray *nx_string_array_read0(FILE *stream)
{
        struct NXStringArray *sa = nx_string_array_alloc();
        if (nx_string_array_read(sa, stream) != 0) {
                nx_string_array_free(sa);
                return NULL;
        }

        return sa;
}

struct NXStringArray *nx_string_array_xread0(FILE *stream)
{
        struct NXStringArray *sa = nx_string_array_alloc();
        nx_string_array_xread(sa, stream);
        return sa;
}
