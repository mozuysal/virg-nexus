/**
 * @file nx_json_tree.h
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
#ifndef VIRG_NEXUS_NX_JSON_TREE_H
#define VIRG_NEXUS_NX_JSON_TREE_H

#include <stdio.h>

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

#define NX_JNT_OBJECT 1
#define NX_JNT_ARRAY 2
#define NX_JNT_STRING 4
#define NX_JNT_INTEGER 8
#define NX_JNT_FPNUMBER 16
#define NX_JNT_TRUE 32
#define NX_JNT_FALSE 64
#define NX_JNT_NULL 128

#define NX_JNT_NUMBER (NX_JNT_INTEGER | NX_JNT_FPNUMBER)
#define NX_JNT_BOOL (NX_JNT_TRUE | NX_JNT_FALSE)

#define NX_JNT_COMPOSITE (NX_JNT_ARRAY | NX_JNT_OBJECT)
#define NX_JNT_INTEGRAL (NX_JNT_NUMBER | NX_JNT_BOOL)
#define NX_JNT_VALUE (NX_JNT_STRING | NX_JNT_INTEGRAL)

struct NXJSONNode;

struct NXJSONNode *nx_json_tree_from_string(char *s);
struct NXJSONNode *nx_json_tree_from_stream(FILE *stream);
struct NXJSONNode *nx_json_tree_from_file(const char *filename);
void nx_json_tree_free(struct NXJSONNode *root);

struct NXJSONNode *nx_json_node_new_object();
struct NXJSONNode *nx_json_node_new_array();
struct NXJSONNode *nx_json_node_new_string(const char *text);
struct NXJSONNode *nx_json_node_new_integer(const char *text);
struct NXJSONNode *nx_json_node_new_fpnumber(const char *text);
struct NXJSONNode *nx_json_node_new_true();
struct NXJSONNode *nx_json_node_new_false();
struct NXJSONNode *nx_json_node_new_null();

int nx_json_node_type(const struct NXJSONNode *node);
NXBool nx_json_node_is(const struct NXJSONNode *node, int type);
NXBool nx_json_node_is_a(const struct NXJSONNode *node, int type);

char *nx_json_node_text(struct NXJSONNode *node);
int nx_json_node_n_children(struct NXJSONNode *node);

struct NXJSONNode *nx_json_array_get(struct NXJSONNode *jarray, int position, int type);
struct NXJSONNode *nx_json_object_get(struct NXJSONNode *jobject, const char *name, int type);

struct NXJSONNode *nx_json_node_add_child(struct NXJSONNode *parent, struct NXJSONNode *node);
struct NXJSONNode *nx_json_array_add(struct NXJSONNode *jarray, struct NXJSONNode *node);
struct NXJSONNode *nx_json_object_add(struct NXJSONNode *jobject, const char *name, struct NXJSONNode *node);

struct NXJSONNode *nx_json_array_fget(struct NXJSONNode *jarray, int position, int type);
struct NXJSONNode *nx_json_object_fget(struct NXJSONNode *jobject, const char *name, int type);

void nx_json_tree_print(struct NXJSONNode *root, int pretty_print_level);
void nx_json_tree_fprint(FILE *stream, struct NXJSONNode *root, int pretty_print_level);

__NX_END_DECL

#endif
