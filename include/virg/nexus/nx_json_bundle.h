/**
 * @file nx_json_bundle.h
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
#ifndef VIRG_NEXUS_NX_JSON_BUNDLE_H
#define VIRG_NEXUS_NX_JSON_BUNDLE_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_keypoint.h"
#include "virg/nexus/nx_string_array.h"
#include "virg/nexus/nx_json_tree.h"

__NX_BEGIN_DECL

struct NXJSONNode* nx_json_bundle_bool(NXBool b);
struct NXJSONNode* nx_json_bundle_char(char c);
struct NXJSONNode* nx_json_bundle_uchar(uchar uc);
struct NXJSONNode* nx_json_bundle_int(int i);
struct NXJSONNode* nx_json_bundle_size_t(size_t z);
struct NXJSONNode* nx_json_bundle_float(float f);
struct NXJSONNode* nx_json_bundle_double(double d);
struct NXJSONNode* nx_json_bundle_string(const char* s);

struct NXJSONNode* nx_json_bundle_int_array(int n, const int* x);
struct NXJSONNode* nx_json_bundle_float_array(int n, const float* x);
struct NXJSONNode* nx_json_bundle_double_array(int n, const double* x);
struct NXJSONNode* nx_json_bundle_string_array(int n, char** x);

struct NXJSONNode* nx_json_bundle_keypoint(const struct NXKeypoint* key);
struct NXJSONNode* nx_json_bundle_keypoint_array(int n, struct NXKeypoint* key);

NXBool nx_json_unbundle_bool  (struct NXJSONNode* node);
char   nx_json_unbundle_char  (struct NXJSONNode* node);
uchar  nx_json_unbundle_uchar (struct NXJSONNode* node);
int    nx_json_unbundle_int   (struct NXJSONNode* node);
size_t nx_json_unbundle_size_t(struct NXJSONNode* node);
float  nx_json_unbundle_float (struct NXJSONNode* node);
double nx_json_unbundle_double(struct NXJSONNode* node);
char*  nx_json_unbundle_string(struct NXJSONNode* node);

int*    nx_json_unbundle_int_array   (struct NXJSONNode* jarray, int* n);
float*  nx_json_unbundle_float_array (struct NXJSONNode* jarray, int* n);
double* nx_json_unbundle_double_array(struct NXJSONNode* jarray, int* n);
char**  nx_json_unbundle_string_array(struct NXJSONNode* jarray, int* n);
struct NXStringArray* nx_json_unbundle_as_string_array(struct NXJSONNode* jarray);

struct NXKeypoint  nx_json_unbundle_keypoint(struct NXJSONNode* node);
struct NXKeypoint *nx_json_unbundle_keypoint_array(struct NXJSONNode* jarray, int* n);

__NX_END_DECL

#endif
