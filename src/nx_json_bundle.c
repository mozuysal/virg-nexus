/**
 * @file nx_json_bundle.c
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
#include "virg/nexus/nx_json_bundle.h"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_string.h"

struct NXJSONNode *nx_json_bundle_bool(NXBool b)
{
        if (b)
                return nx_json_node_new_true();
        else
                return nx_json_node_new_false();
}

struct NXJSONNode *nx_json_bundle_char(char c)
{
        char s[2] = { c, '\0' };
        return nx_json_node_new_string(&s[0]);
}

struct NXJSONNode *nx_json_bundle_uchar(uchar uc)
{
        return nx_json_bundle_int((int)uc);
}

struct NXJSONNode *nx_json_bundle_int(int i)
{
        char *s = nx_fstr("%d", i);
        struct NXJSONNode *n = nx_json_node_new_integer(s);
        nx_free(s);
        return n;
}

struct NXJSONNode *nx_json_bundle_size_t(size_t z)
{
        char *s = nx_fstr("%zd", z);
        struct NXJSONNode *n = nx_json_node_new_integer(s);
        nx_free(s);
        return n;
}

struct NXJSONNode *nx_json_bundle_float(float f)
{
        return nx_json_bundle_double((double)f);
}

struct NXJSONNode *nx_json_bundle_double(double d)
{
        char *s = nx_fstr("%e", d);
        struct NXJSONNode *n = nx_json_node_new_fpnumber(s);
        nx_free(s);
        return n;
}

struct NXJSONNode *nx_json_bundle_string(const char *s)
{
        return nx_json_node_new_string(s);
}

struct NXJSONNode *nx_json_bundle_int_array(int n, const int *x)
{
        struct NXJSONNode *jarray = nx_json_node_new_array();
        for (int i = 0; i < n; ++i)
                nx_json_array_add(jarray, nx_json_bundle_int(x[i]));
        return jarray;
}

struct NXJSONNode *nx_json_bundle_float_array(int n, const float *x)
{
        struct NXJSONNode *jarray = nx_json_node_new_array();
        for (int i = 0; i < n; ++i)
                nx_json_array_add(jarray, nx_json_bundle_float(x[i]));
        return jarray;
}

struct NXJSONNode *nx_json_bundle_double_array(int n, const double *x)
{
        struct NXJSONNode *jarray = nx_json_node_new_array();
        for (int i = 0; i < n; ++i)
                nx_json_array_add(jarray, nx_json_bundle_double(x[i]));
        return jarray;
}

struct NXJSONNode *nx_json_bundle_string_array(int n, char **x)
{
        struct NXJSONNode *jarray = nx_json_node_new_array();
        for (int i = 0; i < n; ++i)
                nx_json_array_add(jarray, nx_json_bundle_string(x[i]));
        return jarray;
}

struct NXJSONNode *nx_json_bundle_keypoint(const struct NXKeypoint* key)
{
        struct NXJSONNode* jkey = nx_json_node_new_object();
        nx_json_object_add(jkey, "x", nx_json_bundle_int(key->x));
        nx_json_object_add(jkey, "y", nx_json_bundle_int(key->y));
        nx_json_object_add(jkey, "xs", nx_json_bundle_double(key->xs));
        nx_json_object_add(jkey, "ys", nx_json_bundle_double(key->ys));
        nx_json_object_add(jkey, "level", nx_json_bundle_int(key->level));
        nx_json_object_add(jkey, "scale", nx_json_bundle_double(key->scale));
        nx_json_object_add(jkey, "sigma", nx_json_bundle_double(key->sigma));
        nx_json_object_add(jkey, "score", nx_json_bundle_double(key->score));
        nx_json_object_add(jkey, "ori", nx_json_bundle_double(key->ori));
        nx_json_object_add(jkey, "id", nx_json_bundle_int(key->id));

        return jkey;
}

struct NXJSONNode *nx_json_bundle_keypoint_array(int n, struct NXKeypoint* key)
{
        struct NXJSONNode *jarray = nx_json_node_new_array();
        for (int i = 0; i < n; ++i)
                nx_json_array_add(jarray, nx_json_bundle_keypoint(&key[i]));
        return jarray;
}


NXBool nx_json_unbundle_bool(struct NXJSONNode *node)
{
        NX_ASSERT_PTR(node);
        NX_ASSERT(nx_json_node_is_a(node, NX_JNT_BOOL));

        return nx_json_node_is(node, NX_JNT_TRUE);
}

char nx_json_unbundle_char(struct NXJSONNode *node)
{
        NX_ASSERT_PTR(node);
        NX_ASSERT(nx_json_node_is_a(node, NX_JNT_STRING));

        return nx_json_node_text(node)[0];
}

uchar nx_json_unbundle_uchar(struct NXJSONNode *node)
{
        int i = nx_json_unbundle_int(node);
        if (i < 0)
                i = 0;
        else if (i > 255)
                i = 255;
        return (uchar)i;
}

int nx_json_unbundle_int(struct NXJSONNode *node)
{
        NX_ASSERT_PTR(node);
        NX_ASSERT(nx_json_node_is_a(node, NX_JNT_INTEGER));
        return atoi(nx_json_node_text(node));
}

size_t nx_json_unbundle_size_t(struct NXJSONNode *node)
{
        NX_ASSERT_PTR(node);
        NX_ASSERT(nx_json_node_is_a(node, NX_JNT_INTEGER));

        return (size_t)atol(nx_json_node_text(node));
}

float nx_json_unbundle_float(struct NXJSONNode *node)
{
        NX_ASSERT_PTR(node);
        NX_ASSERT(nx_json_node_is_a(node, NX_JNT_NUMBER));

        return atof(nx_json_node_text(node));
}

double nx_json_unbundle_double(struct NXJSONNode *node)
{
        NX_ASSERT_PTR(node);
        NX_ASSERT(nx_json_node_is_a(node, NX_JNT_NUMBER));

        return atof(nx_json_node_text(node));
}

char *nx_json_unbundle_string(struct NXJSONNode *node)
{
        NX_ASSERT_PTR(node);
        NX_ASSERT(nx_json_node_is_a(node, NX_JNT_STRING));

        return nx_strdup(nx_json_node_text(node));
}

int *nx_json_unbundle_int_array(struct NXJSONNode *jarray, int *n)
{
        NX_ASSERT_PTR(jarray);
        NX_ASSERT(nx_json_node_is_a(jarray, NX_JNT_ARRAY));

        *n = nx_json_node_n_children(jarray);
        int *ia = NX_NEW_I(*n);
        for (int i = 0; i < *n; ++i)
                ia[i] = nx_json_unbundle_int(nx_json_array_fget(jarray, i, NX_JNT_INTEGER));
        return ia;
}

float *nx_json_unbundle_float_array(struct NXJSONNode *jarray, int *n)
{
        NX_ASSERT_PTR(jarray);
        NX_ASSERT(nx_json_node_is_a(jarray, NX_JNT_ARRAY));

        *n = nx_json_node_n_children(jarray);
        float *fa = NX_NEW_S(*n);
        for (int i = 0; i < *n; ++i)
                fa[i] = nx_json_unbundle_float(nx_json_array_fget(jarray, i, NX_JNT_NUMBER));
        return fa;
}

double *nx_json_unbundle_double_array(struct NXJSONNode *jarray, int *n)
{
        NX_ASSERT_PTR(jarray);
        NX_ASSERT(nx_json_node_is_a(jarray, NX_JNT_ARRAY));

        *n = nx_json_node_n_children(jarray);
        double *da = NX_NEW_D(*n);
        for (int i = 0; i < *n; ++i)
                da[i] = nx_json_unbundle_double(nx_json_array_fget(jarray, i, NX_JNT_NUMBER));
        return da;
}

char **nx_json_unbundle_string_array(struct NXJSONNode *jarray, int *n)
{
        NX_ASSERT_PTR(jarray);
        NX_ASSERT(nx_json_node_is_a(jarray, NX_JNT_ARRAY));

        *n = nx_json_node_n_children(jarray);
        char **sa = NX_NEW(*n, char *);
        for (int i = 0; i < *n; ++i)
                sa[i] = nx_json_unbundle_string(nx_json_array_fget(jarray, i, NX_JNT_STRING));
        return sa;
}

struct NXStringArray *nx_json_unbundle_as_string_array(struct NXJSONNode *jarray)
{
        NX_ASSERT_PTR(jarray);
        NX_ASSERT(nx_json_node_is_a(jarray, NX_JNT_ARRAY));

        int n = nx_json_node_n_children(jarray);
        struct NXStringArray *sa = nx_string_array_new(n);
        for (int i = 0; i < n; ++i) {
                struct NXJSONNode *e = nx_json_array_fget(jarray, i, NX_JNT_STRING);
                nx_string_array_set(sa, i, nx_json_node_text(e));
        }
        return sa;
}

struct NXKeypoint nx_json_unbundle_keypoint(struct NXJSONNode* node)
{
        NX_ASSERT_PTR(node);
        NX_ASSERT(nx_json_node_is_a(node, NX_JNT_OBJECT));

        struct NXKeypoint key;
        key.x = nx_json_unbundle_int(nx_json_object_fget(node, "x", NX_JNT_INTEGER));
        key.y = nx_json_unbundle_int(nx_json_object_fget(node, "y", NX_JNT_INTEGER));
        key.xs = nx_json_unbundle_double(nx_json_object_fget(node, "xs", NX_JNT_FPNUMBER));
        key.ys = nx_json_unbundle_double(nx_json_object_fget(node, "ys", NX_JNT_FPNUMBER));
        key.level = nx_json_unbundle_int(nx_json_object_fget(node, "level", NX_JNT_INTEGER));
        key.scale = nx_json_unbundle_double(nx_json_object_fget(node, "scale", NX_JNT_FPNUMBER));
        key.sigma = nx_json_unbundle_double(nx_json_object_fget(node, "sigma",  NX_JNT_FPNUMBER));
        key.score = nx_json_unbundle_double(nx_json_object_fget(node, "score",  NX_JNT_FPNUMBER));
        key.ori = nx_json_unbundle_double(nx_json_object_fget(node, "ori",  NX_JNT_FPNUMBER));
        key.id = nx_json_unbundle_int(nx_json_object_fget(node, "id",  NX_JNT_INTEGER));

        return key;
}

struct NXKeypoint *nx_json_unbundle_keypoint_array(struct NXJSONNode* jarray, int* n)
{
        NX_ASSERT_PTR(jarray);
        NX_ASSERT(nx_json_node_is_a(jarray, NX_JNT_ARRAY));

        *n = nx_json_node_n_children(jarray);
        struct NXKeypoint* keys = NX_NEW(*n, struct NXKeypoint);
        for (int i = 0; i < *n; ++i)
                keys[i] = nx_json_unbundle_keypoint(nx_json_array_fget(jarray, i, NX_JNT_OBJECT));
        return keys;

}
