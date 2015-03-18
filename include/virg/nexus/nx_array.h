/**
 * @file nx_array.h
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
#ifndef VIRG_NEXUS_NX_ARRAY_H
#define VIRG_NEXUS_NX_ARRAY_H

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

#include <stdlib.h>

struct NXArray;

struct NXArray *nx_array_alloc(size_t elem_size);
struct NXArray *nx_array_new(size_t elem_size, int initial_size);
void nx_array_free(struct NXArray *arr);

int nx_array_size(const struct NXArray *arr);
void nx_array_resize(struct NXArray *arr, int new_size);
void *nx_array_data(struct NXArray *arr);

int nx_array_capacity(const struct NXArray *arr);
void nx_array_reserve(struct NXArray *arr, int new_capacity);
void nx_array_cap_to_size(struct NXArray *arr);

void nx_array_set(struct NXArray *arr, int id, void *value);
void *nx_array_get(const struct NXArray *arr, int id);
void nx_array_push_back(struct NXArray *arr, void *value);

__NX_END_DECL

#endif
