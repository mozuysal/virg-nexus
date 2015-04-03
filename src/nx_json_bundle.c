/**
 * @file nx_json_bundle.c
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
