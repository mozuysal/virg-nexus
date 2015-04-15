/**
 * @file nx_json_tree.c
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
#include "virg/nexus/nx_json_tree.h"

#include <string.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_message.h"
#include "virg/nexus/nx_string.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_io.h"
#include "virg/nexus/nx_json_parser.h"

#define JSON_TAB_WIDTH 4

struct NXJSONNode {
        int type;
        char *text;

        struct NXJSONNode *right;
        struct NXJSONNode *down;
        struct NXJSONNode *last_child;
};

struct NXJSONNode *nx_json_tree_from_string(char *s)
{
        struct NXJSONLexer *jlex = nx_json_lexer_new(s);
        struct NXJSONParser *jp = nx_json_parser_new(jlex);
        struct NXJSONNode *json_tree = nx_json_parser_parse(jp);

        nx_json_lexer_free(jlex);
        nx_json_parser_free(jp);
        return json_tree;
}

struct NXJSONNode *nx_json_tree_from_stream(FILE *stream)
{
        int cap = 1024;
        char *jtext = NX_NEW_C(cap);

        int c;
        int i = 0;
        while ((c = fgetc(stream)) != EOF) {
                jtext[i++] = c;

                if (i == cap-1) {
                        cap *= 2;
                        jtext = nx_frealloc(jtext, cap);
                }
        }
        jtext[i] = '\0';

        struct NXJSONNode *json_tree = nx_json_tree_from_string(jtext);
        nx_free(jtext);

        return json_tree;
}

struct NXJSONNode *nx_json_tree_from_file(const char *filename)
{
        FILE *jin = nx_xfopen(filename, "r");
        struct NXJSONNode *json_tree = nx_json_tree_from_stream(jin);
        nx_xfclose(jin, filename);

        return json_tree;
}

static void nx_fputs_readable(const char *s, FILE *stream)
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

static const char *nx_json_node_type_name(int type)
{
        switch (type) {
        case NX_JNT_OBJECT: return "object";
        case NX_JNT_ARRAY: return "array";
        case NX_JNT_STRING: return "string";
        case NX_JNT_INTEGER: return "integer";
        case NX_JNT_FPNUMBER: return "fpnumber";
        case NX_JNT_TRUE: return "true";
        case NX_JNT_FALSE: return "false";
        case NX_JNT_NULL: return "null";
        case NX_JNT_NUMBER: return "integer | fpnumber";
        case NX_JNT_BOOL: return "true | false";
        case NX_JNT_COMPOSITE: return "object | array";
        case NX_JNT_INTEGRAL: return "integer | fpnumber | true | false";
        case NX_JNT_VALUE: return "string| integer | fpnumber | true | false";

        case (NX_JNT_OBJECT | NX_JNT_NULL): return "object | null";
        case (NX_JNT_ARRAY | NX_JNT_NULL): return "array | null";
        case (NX_JNT_STRING | NX_JNT_NULL): return "string | null";
        case (NX_JNT_INTEGER | NX_JNT_NULL): return "integer | null";
        case (NX_JNT_FPNUMBER | NX_JNT_NULL): return "fpnumber | null";
        case (NX_JNT_TRUE | NX_JNT_NULL): return "true | null";
        case (NX_JNT_FALSE | NX_JNT_NULL): return "false | null";
        case (NX_JNT_NUMBER | NX_JNT_NULL): return "integer | fpnumber | null";
        case (NX_JNT_BOOL | NX_JNT_NULL): return "true | false | null";
        case (NX_JNT_COMPOSITE | NX_JNT_NULL): return "object | array | null";
        case (NX_JNT_INTEGRAL | NX_JNT_NULL): return "integer | fpnumber | true | false | null";
        case (NX_JNT_VALUE | NX_JNT_NULL): return "string| integer | fpnumber | true | false | null";
        default:
                return "type?";
        }
}

static struct NXJSONNode *nx_json_node_alloc()
{
        struct NXJSONNode *n = NX_NEW(1, struct NXJSONNode);
        n->text = NULL;
        n->right = NULL;
        n->down = NULL;
        n->last_child = NULL;
        return n;
}

static struct NXJSONNode *nx_json_node_new_composite(int type)
{
        struct NXJSONNode *n = nx_json_node_alloc();
        n->type = type;
        return n;
}

static struct NXJSONNode *nx_json_node_new_constant(int type)
{
        struct NXJSONNode *n = nx_json_node_alloc();
        n->type = type;
        return n;
}

static struct NXJSONNode *nx_json_node_new_variable(int type, const char *text)
{
        struct NXJSONNode *n = nx_json_node_alloc();
        n->type = type;
        n->text = nx_strdup(text);
        return n;
}

struct NXJSONNode *nx_json_node_new_object()
{
        return nx_json_node_new_composite(NX_JNT_OBJECT);
}

struct NXJSONNode *nx_json_node_new_array()
{
        return nx_json_node_new_composite(NX_JNT_ARRAY);
}

struct NXJSONNode *nx_json_node_new_string(const char *text)
{
        return nx_json_node_new_variable(NX_JNT_STRING, text);
}

struct NXJSONNode *nx_json_node_new_integer(const char *text)
{
        return nx_json_node_new_variable(NX_JNT_INTEGER, text);
}

struct NXJSONNode *nx_json_node_new_fpnumber(const char *text)
{
        return nx_json_node_new_variable(NX_JNT_FPNUMBER, text);
}

struct NXJSONNode *nx_json_node_new_true()
{
        return nx_json_node_new_constant(NX_JNT_TRUE);
}

struct NXJSONNode *nx_json_node_new_false()
{
        return nx_json_node_new_constant(NX_JNT_TRUE);
}

struct NXJSONNode *nx_json_node_new_null()
{
        return nx_json_node_new_constant(NX_JNT_TRUE);
}

void nx_json_tree_free(struct NXJSONNode *root)
{
        if (root != NULL) {
                nx_json_tree_free(root->down);
                nx_json_tree_free(root->right);
                nx_free(root->text);
                nx_free(root);
        }
}

int nx_json_node_type(const struct NXJSONNode *node)
{
        return node->type;
}

NXBool nx_json_node_is(const struct NXJSONNode *node, int type)
{
        return node != NULL && (node->type == type);
}

NXBool nx_json_node_is_a(const struct NXJSONNode *node, int type)
{
        return node != NULL && (node->type & type) != 0;
}

char *nx_json_node_text(struct NXJSONNode *node)
{
        return node->text;
}

int nx_json_node_n_children(struct NXJSONNode *node)
{
        int n = 0;
        struct NXJSONNode *child = node->down;
        while (child) {
                child = child->right;
                ++n;
        }
        return n;
}

struct NXJSONNode *nx_json_array_get(struct NXJSONNode *jarray, int position, int type)
{
        if (!nx_json_node_is(jarray, NX_JNT_ARRAY))
            return NULL;

        struct NXJSONNode *n = jarray->down;
        if (n == NULL)
                return NULL;

        while (n != NULL && position-- > 0)
                n = n->right;

        if (!nx_json_node_is_a(n, type))
                return NULL;

        return n;
}

struct NXJSONNode *nx_json_object_get(struct NXJSONNode *jobject, const char *name, int type)
{
        if (!nx_json_node_is(jobject, NX_JNT_OBJECT))
            return NULL;

        struct NXJSONNode *n = jobject->down;
        if (n == NULL)
                return NULL;

        while (n != NULL && strcmp(n->text, name) != 0)
                n = n->right;

        if (n == NULL)
                return NULL;
        n = n->right;

        if (!nx_json_node_is_a(n, type))
                return NULL;

        return n;
}

struct NXJSONNode *nx_json_array_fget(struct NXJSONNode *jarray, int position, int type)
{
        struct NXJSONNode *n = nx_json_array_get(jarray, position, type);
        if (n == NULL)
                nx_fatal(NX_LOG_TAG, "JSON array has no element matching type <%s> at position %d!",
                         nx_json_node_type_name(type), position);

        return n;
}

struct NXJSONNode *nx_json_object_fget(struct NXJSONNode *jobject, const char *name, int type)
{
        struct NXJSONNode *n = nx_json_object_get(jobject, name, type);
        if (n == NULL)
                nx_fatal(NX_LOG_TAG, "JSON object has no element matching type <%s> with name '%s'!",
                         nx_json_node_type_name(type), name);

        return n;
}

void nx_json_tree_print(struct NXJSONNode *root, int pretty_print_level)
{
        nx_json_tree_fprint(stdout, root, pretty_print_level);
}

static inline void make_tab(FILE *stream, int width)
{
        for (int i = 0; i < width; ++i)
                fputc(' ', stream);
}

static void nx_json_tree_fprint_recursive(FILE *stream, struct NXJSONNode *root, int pretty_print_level, int tab_width)
{
        struct NXJSONNode *n = NULL;
        const char *pp_str = (pretty_print_level > 0) ? "\n" : " ";
        int tab_width_children = (pretty_print_level > 0) ? tab_width + JSON_TAB_WIDTH : 0;

        if (root == NULL) {
                return;
        }

        switch (root->type) {
        case NX_JNT_NULL: fputs("null", stream); return;
        case NX_JNT_TRUE: fputs("true", stream); return;
        case NX_JNT_FALSE: fputs("false", stream); return;
        case NX_JNT_STRING: nx_fputs_readable(root->text, stream); return;
        case NX_JNT_FPNUMBER:
        case NX_JNT_INTEGER: fprintf(stream, "%s", root->text); return;
        case NX_JNT_ARRAY:
                fprintf(stream, "[%s", pp_str);
                make_tab(stream, tab_width_children);
                n = root->down;
                while (n != NULL) {
                        nx_json_tree_fprint_recursive(stream, n, pretty_print_level-1,
                                                      tab_width + JSON_TAB_WIDTH);
                        n = n->right;
                        if (n != NULL) {
                                fprintf(stream, ",%s", pp_str);
                                make_tab(stream, tab_width_children);
                        }

                }
                fprintf(stream, "%s", pp_str);
                make_tab(stream, tab_width_children - JSON_TAB_WIDTH);
                fputc(']', stream);
                return;
        case NX_JNT_OBJECT:
                fprintf(stream, "{%s", pp_str);
                make_tab(stream, tab_width_children);
                n = root->down;
                while (n != NULL) {
                        NX_ASSERT(n->type == NX_JNT_STRING);
                        nx_fputs_readable(n->text, stream);
                        fputs(" : ", stream);
                        n = n->right;
                        NX_ASSERT_PTR(n);
                        nx_json_tree_fprint_recursive(stream, n, pretty_print_level-1,
                                                      tab_width + JSON_TAB_WIDTH);
                        n = n->right;
                        if (n != NULL) {
                                fprintf(stream, ",%s", pp_str);
                                make_tab(stream, tab_width_children);
                        }
                }
                fprintf(stream, "%s", pp_str);
                make_tab(stream, tab_width_children - JSON_TAB_WIDTH);
                fputc('}', stream);
                return;
        default:
                nx_fatal(NX_LOG_TAG, "Unknown JSON node type %d while printing!", root->type);
        }
}

void nx_json_tree_fprint(FILE *stream, struct NXJSONNode *root, int pretty_print_level)
{
        nx_json_tree_fprint_recursive(stream, root, pretty_print_level, 0);
        fputs("\n", stream);
}

struct NXJSONNode *nx_json_node_add_child(struct NXJSONNode *parent, struct NXJSONNode *node)
{
        if (parent->last_child == NULL) {
                parent->down = node;
                parent->last_child = node;
        } else {
                parent->last_child->right = node;
        }

        while (parent->last_child->right != NULL)
                parent->last_child = parent->last_child->right;

        return node;
}

struct NXJSONNode *nx_json_array_add(struct NXJSONNode *jarray, struct NXJSONNode *node)
{
        NX_ASSERT_PTR(jarray);
        NX_ASSERT_PTR(node);
        NX_ASSERT(jarray->type == NX_JNT_ARRAY);

        return nx_json_node_add_child(jarray, node);
}

struct NXJSONNode *nx_json_object_add(struct NXJSONNode *jobject, const char *name, struct NXJSONNode *node)
{
        NX_ASSERT_PTR(jobject);
        NX_ASSERT_PTR(node);
        NX_ASSERT(jobject->type == NX_JNT_OBJECT);

        struct NXJSONNode *name_node = nx_json_node_new_string(name);
        name_node->right = node;
        nx_json_node_add_child(jobject, name_node);
        return node;
}
