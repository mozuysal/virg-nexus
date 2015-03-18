/**
 * @file nx_array.c
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
#include "virg/nexus/nx_array.h"

#include <string.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_message.h"

struct NXArray
{
        int size;
        int capacity;
        size_t elem_sz;
        char *elems;
};

struct NXArray *nx_array_alloc(size_t elem_size)
{
        struct NXArray *arr = NX_NEW(1, struct NXArray);
        arr->size = 0;
        arr->capacity = 0;
        arr->elem_sz = elem_size;
        arr->elems = NULL;

        return arr;
}

struct NXArray *nx_array_new(size_t elem_size, int initial_size)
{
        struct NXArray *arr = nx_array_alloc(elem_size);
        nx_array_resize(arr, initial_size);
        return arr;
}

void nx_array_free(struct NXArray *arr)
{
        if (arr) {
                nx_free(arr->elems);
                nx_free(arr);
        }
}

int nx_array_size(const struct NXArray *arr)
{
        return arr->size;
}

void nx_array_resize(struct NXArray *arr, int new_size)
{
        while (arr->capacity < new_size)
                nx_array_reserve(arr, 2 * arr->capacity);

        if (new_size < arr->capacity / 2)
                nx_array_reserve(arr, arr->capacity / 2);

        arr->size = new_size;
}

void *nx_array_data(struct NXArray *arr)
{
        return arr->elems;
}

int nx_array_capacity(const struct NXArray *arr)
{
        return arr->capacity;
}

void nx_array_reserve(struct NXArray *arr, int new_capacity)
{
        if (new_capacity < arr->size)
                new_capacity = arr->size;

        arr->elems = nx_frealloc(arr->elems, new_capacity * arr->elem_sz);
        arr->capacity = new_capacity;
}

void nx_array_cap_to_size(struct NXArray *arr)
{
        nx_array_reserve(arr, arr->size);
}

void nx_array_set(struct NXArray *arr, int id, void *value)
{
        if (id >= arr->size)
                nx_array_resize(arr, id+1);
        memmove(arr->elems + id * arr->elem_sz, value, arr->elem_sz);
}

void *nx_array_get(const struct NXArray *arr, int id)
{
        return arr->elems + id * arr->elem_sz;
}

void nx_array_push_back(struct NXArray *arr, void *value)
{
        nx_array_set(arr, arr->size, value);
}

