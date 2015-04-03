/**
 * @file nx_json_tree.h
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

void nx_json_tree_print(struct NXJSONNode *root, int pretty_print_level);
void nx_json_tree_fprint(FILE *stream, struct NXJSONNode *root, int pretty_print_level);

__NX_END_DECL

#endif
