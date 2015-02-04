/**
 * @file nx_json_options.c
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
#include "virg/nexus/nx_json_options.h"

#include <ctype.h>
#include <string.h>

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_message.h"
#include "virg/nexus/nx_io.h"

#include "jsmn/jsmn.h"

#define LOG_TAG "NXJSONOptions"
#define JOPTION_MAP_SIZE 128

#define JOUT_NEWLINE "\n"
#define JOUT_INDENT_STRING "  "
#define JOUT_ITEM_DELIMITER ","
#define JOUT_NAME_DELIMITER " : "
#define JOUT_DOUBLE_FORMAT "%.6f"
#define JOUT_BEGIN "{" JOUT_NEWLINE
#define JOUT_END "}" JOUT_NEWLINE
#define JOUT_ARRAY_START "["
#define JOUT_ARRAY_END " ]"

#define READER_BUFFER_INITIAL_SIZE 1024
#define READER_INITIAL_N_TOKENS 256

#define TOKEN_LENGTH(t) ((t)->end - (t)->start)

#define STRDUP(dest,src) do {                           \
                if ((dest) != NULL)                     \
                        nx_free(dest);                  \
                (dest) = NX_NEW_C(strlen(src)+1);       \
                strcpy((dest), (src));                  \
        } while(0)

struct NXJSONReader {
        jsmn_parser parser;

        int max_n_tokens;
        int n_tokens;
        jsmntok_t* tokens;

        size_t buffer_size;
        char* buffer;
};

static struct NXJSONReader* nx_json_reader_new();
static void nx_json_reader_free(struct NXJSONReader* reader);
static void nx_json_reader_read_stream(struct NXJSONReader* reader, FILE* stream);
static void nx_json_reader_resize_tokens(struct NXJSONReader* reader);
static NXBool nx_json_reader_parse_stream(struct NXJSONReader* reader, FILE* stream);

enum NXJOptionType {
        NX_JOT_Bool,
        NX_JOT_Int,
        NX_JOT_Double,
        NX_JOT_String,
        NX_JOT_Bool_Array,
        NX_JOT_Int_Array,
        NX_JOT_Double_Array,
        NX_JOT_String_Array
};

union NXJOptionValue {
        NXBool b;
        int i;
        double d;
        char *s;
        NXBool *ba;
        int *ia;
        double *da;
        char **sa;
};

struct NXJOptionData {
        char *name;
        enum NXJOptionType type;
        union NXJOptionValue value;
        size_t array_length;
        NXBool is_required;
        NXBool is_set;
        union NXJOptionValue default_value;

        struct NXJOptionData *next;
};

static struct NXJOptionData *nx_joption_data_alloc();
static struct NXJOptionData *nx_joption_data_new(const char *name, NXBool is_required);
static struct NXJOptionData *nx_joption_data_new_bool(const char *name, NXBool is_required, NXBool def_val);
static struct NXJOptionData *nx_joption_data_new_int(const char *name, NXBool is_required, int def_val);
static struct NXJOptionData *nx_joption_data_new_double(const char *name, NXBool is_required, double def_val);
static struct NXJOptionData *nx_joption_data_new_string(const char *name, NXBool is_required, const char *def_val);
static struct NXJOptionData *nx_joption_data_new_array(const char *name, enum NXJOptionType type, NXBool is_required);

static void nx_joption_data_realloc_value(struct NXJOptionData *jdata, size_t n);
static void nx_joption_data_free(struct NXJOptionData *jdata);
static void nx_joption_data_free_value(struct NXJOptionData *jdata);
static void nx_joption_data_print(const struct NXJOptionData *jdata, FILE *stream, int name_col_width, NXBool has_next);
static void nx_joption_data_print_array_items(const struct NXJOptionData *jdata, FILE *stream);
static void nx_joption_data_set_from_default_value(struct NXJOptionData *jdata);
static int nx_joption_data_set_from_token(struct NXJOptionData *jdata, const struct NXJSONReader *jreader, int token_id);
static void nx_joption_data_set_from_simple_token(struct NXJOptionData *jdata, const struct NXJSONReader *jreader, int token_id);
static void nx_joption_data_set_from_primitive_token(struct NXJOptionData *jdata, const char *token_text);
static void nx_joption_data_set_array_element_from_primitive_token(struct NXJOptionData *jdata, int elem_id, const char *token_text);

struct NXJOptionMapNode {
        char *key;
        struct NXJOptionData *data;
        struct NXJOptionMapNode *next;
};

static void nx_joption_map_node_free(struct NXJOptionMapNode *jmnode);
static struct NXJOptionMapNode *nx_joption_map_node_new(const char *key, struct NXJOptionData *jdata);

struct NXJSONOptions
{
        int n_jopt;
        struct {
                struct NXJOptionData *head;
                struct NXJOptionData *tail;
        } jopt_list;
        struct NXJOptionMapNode **jopt_map;
};

static void nx_json_options_free_list(struct NXJSONOptions *jopt);
static void nx_json_options_free_map(struct NXJSONOptions *jopt);

static void nx_json_options_vadd(struct NXJSONOptions *jopt, const char *opt_format, va_list opt_defs);

static void nx_json_options_add_data(struct NXJSONOptions *jopt, struct NXJOptionData *jdata);
static void nx_json_options_add_data_to_list(struct NXJSONOptions *jopt, struct NXJOptionData *jdata);
static void nx_json_options_add_data_to_map(struct NXJSONOptions *jopt, struct NXJOptionData *jdata);
static struct NXJOptionData *nx_json_options_find_in_map(struct NXJSONOptions *jopt, const char *name);

static int nx_json_options_hash_name(const char *name);

struct NXJSONOptions *nx_json_options_alloc()
{
        struct NXJSONOptions *jopt = NX_NEW(1, struct NXJSONOptions);

        jopt->n_jopt = 0;
        jopt->jopt_list.head = NULL;
        jopt->jopt_list.tail = NULL;

        jopt->jopt_map = NX_NEW(JOPTION_MAP_SIZE, struct NXJOptionMapNode *);
        for (int i = 0; i < JOPTION_MAP_SIZE; ++i)
                jopt->jopt_map[i] = NULL;

        return jopt;
}

struct NXJSONOptions *nx_json_options_new(char *opt_format, ...)
{
        struct NXJSONOptions *jopt = nx_json_options_alloc();

        va_list opt_defs;
        va_start(opt_defs, opt_format);
        nx_json_options_vadd(jopt, opt_format, opt_defs);
        va_end(opt_defs);

        return jopt;
}

void nx_json_options_free(struct NXJSONOptions *jopt)
{
        if (jopt) {
                nx_json_options_free_list(jopt);
                nx_json_options_free_map(jopt);
                nx_free(jopt);
        }
}

void nx_json_options_free_list(struct NXJSONOptions *jopt)
{
        struct NXJOptionData *it = jopt->jopt_list.head;
        while (it != NULL) {
                struct NXJOptionData *to_del = it;
                it = it->next;
                nx_joption_data_free(to_del);
        }
        jopt->jopt_list.tail = NULL;
}

void nx_json_options_free_map(struct NXJSONOptions *jopt)
{
        for (int i = 0; i < JOPTION_MAP_SIZE; ++i) {
                while (jopt->jopt_map[i] != NULL) {
                        struct NXJOptionMapNode *to_del = jopt->jopt_map[i];
                        jopt->jopt_map[i] = to_del->next;
                        nx_joption_map_node_free(to_del);
                }
        }

        nx_free(jopt->jopt_map);
        jopt->jopt_map = NULL;
}

void nx_json_options_add(struct NXJSONOptions *jopt, const char *opt_format, ...)
{
        NX_ASSERT_PTR(jopt);

        va_list opt_defs;
        va_start(opt_defs, opt_format);
        nx_json_options_vadd(jopt, opt_format, opt_defs);
        va_end(opt_defs);
}

static const char *get_option_info(const char *fmt, enum NXJOptionType *type, NXBool *is_required, NXBool *is_array)
{
        if (fmt[0] == '\0')
                return NULL;

        char c = fmt[0];
        *is_required = isupper(c);
        c = tolower(c);
        *is_array = (fmt[1] == '*');

        if (*is_array) {
                switch (c) {
                case 'b': *type = NX_JOT_Bool_Array; return fmt+2;
                case 'i': *type = NX_JOT_Int_Array; return fmt+2;
                case 'd': *type = NX_JOT_Double_Array; return fmt+2;
                case 's': *type = NX_JOT_String_Array; return fmt+2;
                default:
                        break;
                }
        } else {
                switch (c) {
                case 'b': *type = NX_JOT_Bool; return fmt+1;
                case 'i': *type = NX_JOT_Int; return fmt+1;
                case 'd': *type = NX_JOT_Double; return fmt+1;
                case 's': *type = NX_JOT_String; return fmt+1;
                default:
                        break;
                }
        }

        nx_fatal(LOG_TAG, "Unexpected character processing option format '%s'", fmt);
        return NULL;
}

void nx_json_options_vadd(struct NXJSONOptions *jopt, const char *opt_format, va_list opt_defs)
{
        if (opt_format == NULL)
                return;

        enum NXJOptionType type;
        NXBool is_required;
        NXBool is_array;
        while ((opt_format = get_option_info(opt_format, &type, &is_required, &is_array)) != NULL) {
                struct NXJOptionData *jdata = NULL;
                char *opt_name = va_arg(opt_defs, char *);

                if (is_array) {
                        jdata = nx_joption_data_new_array(opt_name, type, is_required);
                } else {
                        switch (type) {
                        case NX_JOT_Bool: jdata = nx_joption_data_new_bool(opt_name, is_required, va_arg(opt_defs, NXBool)); break;
                        case NX_JOT_Int: jdata = nx_joption_data_new_int(opt_name, is_required, va_arg(opt_defs, int)); break;
                        case NX_JOT_Double: jdata = nx_joption_data_new_double(opt_name, is_required, va_arg(opt_defs, double)); break;
                        case NX_JOT_String: jdata = nx_joption_data_new_string(opt_name, is_required, va_arg(opt_defs, char *)); break;
                        default:
                                break;
                        }
                }

                nx_json_options_add_data(jopt, jdata);
        }
}

void nx_json_options_add_data(struct NXJSONOptions *jopt, struct NXJOptionData *jdata)
{
        if (nx_json_options_find_in_map(jopt, jdata->name) != NULL)
                nx_fatal(LOG_TAG, "Can not add duplicate JSON option '%s'", jdata->name);

        nx_json_options_add_data_to_list(jopt, jdata);
        nx_json_options_add_data_to_map(jopt, jdata);
        ++jopt->n_jopt;
}

void nx_json_options_add_data_to_list(struct NXJSONOptions *jopt, struct NXJOptionData *jdata)
{
        jdata->next = NULL;
        if (jopt->jopt_list.head == NULL) {
                jopt->jopt_list.head = jdata;
                jopt->jopt_list.tail = jdata;
        } else {
                jopt->jopt_list.tail->next = jdata;
                jopt->jopt_list.tail = jdata;
        }
}

void nx_json_options_add_data_to_map(struct NXJSONOptions *jopt, struct NXJOptionData *jdata)
{
        int bid = abs(nx_json_options_hash_name(jdata->name) % JOPTION_MAP_SIZE);
        struct NXJOptionMapNode *node = nx_joption_map_node_new(jdata->name, jdata);
        node->next = jopt->jopt_map[bid];
        jopt->jopt_map[bid] = node;
}

int nx_json_options_hash_name(const char *name)
{
        int hash = 0;
        while (*name != '\0') {
                hash = hash * 31 + *name;
                ++name;
        }
        return hash;
}

struct NXJOptionData *nx_json_options_find_in_map(struct NXJSONOptions *jopt, const char *name)
{
        if (name == NULL)
                return NULL;

        int bid = abs(nx_json_options_hash_name(name) % JOPTION_MAP_SIZE);
        struct NXJOptionMapNode *node = jopt->jopt_map[bid];
        while (node != NULL) {
                if (strcmp(node->key, name) == 0) {
                        return node->data;
                }
                node = node->next;
        }

        return NULL;
}

NXBool nx_json_options_get_bool(struct NXJSONOptions *jopt, const char *opt_name)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(opt_name);

        struct NXJOptionData *jdata = nx_json_options_find_in_map(jopt, opt_name);
        if (!jdata)
                nx_fatal(LOG_TAG, "Can not find option: %s", opt_name);

        if (jdata->type != NX_JOT_Bool)
                nx_fatal(LOG_TAG, "Error fetching option %s as boolean", opt_name);

        return jdata->value.b;
}

int nx_json_options_get_int(struct NXJSONOptions *jopt, const char *opt_name)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(opt_name);

        struct NXJOptionData *jdata = nx_json_options_find_in_map(jopt, opt_name);
        if (!jdata)
                nx_fatal(LOG_TAG, "Can not find option: %s", opt_name);

        if (jdata->type != NX_JOT_Int)
                nx_fatal(LOG_TAG, "Error fetching option %s as int", opt_name);

        return jdata->value.i;
}

double nx_json_options_get_double(struct NXJSONOptions *jopt, const char *opt_name)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(opt_name);

        struct NXJOptionData *jdata = nx_json_options_find_in_map(jopt, opt_name);
        if (!jdata)
                nx_fatal(LOG_TAG, "Can not find option: %s", opt_name);

        if (jdata->type != NX_JOT_Double)
                nx_fatal(LOG_TAG, "Error fetching option %s as double", opt_name);

        return jdata->value.d;
}

const char *nx_json_options_get_string(struct NXJSONOptions *jopt, const char *opt_name)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(opt_name);

        struct NXJOptionData *jdata = nx_json_options_find_in_map(jopt, opt_name);
        if (!jdata)
                nx_fatal(LOG_TAG, "Can not find option: %s", opt_name);

        if (jdata->type != NX_JOT_String)
                nx_fatal(LOG_TAG, "Error fetching option %s as string", opt_name);

        return jdata->value.s;
}

void nx_json_options_set_bool(struct NXJSONOptions *jopt, const char *opt_name, NXBool value)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(opt_name);

        struct NXJOptionData *jdata = nx_json_options_find_in_map(jopt, opt_name);
        if (!jdata)
                nx_fatal(LOG_TAG, "Can not find option: %s", opt_name);

        if (jdata->type != NX_JOT_Bool)
                nx_fatal(LOG_TAG, "Error fetching option %s as boolean", opt_name);

        jdata->value.b = value;
        jdata->is_set = NX_TRUE;
}

void nx_json_options_set_int(struct NXJSONOptions *jopt, const char *opt_name, int value)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(opt_name);

        struct NXJOptionData *jdata = nx_json_options_find_in_map(jopt, opt_name);
        if (!jdata)
                nx_fatal(LOG_TAG, "Can not find option: %s", opt_name);

        if (jdata->type != NX_JOT_Int)
                nx_fatal(LOG_TAG, "Error fetching option %s as int", opt_name);

        jdata->value.i = value;
        jdata->is_set = NX_TRUE;
}

void nx_json_options_set_double(struct NXJSONOptions *jopt, const char *opt_name, double value)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(opt_name);

        struct NXJOptionData *jdata = nx_json_options_find_in_map(jopt, opt_name);
        if (!jdata)
                nx_fatal(LOG_TAG, "Can not find option: %s", opt_name);

        if (jdata->type != NX_JOT_Double)
                nx_fatal(LOG_TAG, "Error fetching option %s as double", opt_name);

        jdata->value.d = value;
        jdata->is_set = NX_TRUE;
}

void nx_json_options_set_string(struct NXJSONOptions *jopt, const char *opt_name, const char *value)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(opt_name);

        struct NXJOptionData *jdata = nx_json_options_find_in_map(jopt, opt_name);
        if (!jdata)
                nx_fatal(LOG_TAG, "Can not find option: %s", opt_name);

        if (jdata->type != NX_JOT_String)
                nx_fatal(LOG_TAG, "Error fetching option %s as string", opt_name);

        STRDUP(jdata->value.s, value);
        jdata->is_set = NX_TRUE;
}

void nx_json_options_set_bool_array(struct NXJSONOptions *jopt, const char *opt_name, size_t length, const NXBool *value)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(opt_name);

        struct NXJOptionData *jdata = nx_json_options_find_in_map(jopt, opt_name);
        if (!jdata)
                nx_fatal(LOG_TAG, "Can not find option: %s", opt_name);

        if (jdata->type != NX_JOT_Bool_Array)
                nx_fatal(LOG_TAG, "Error fetching option %s as Boolean array", opt_name);

        nx_joption_data_realloc_value(jdata, length);
        for (size_t i = 0; i < length; ++i)
                jdata->value.ba[i] = value[i];
}

void nx_json_options_set_int_array(struct NXJSONOptions *jopt, const char *opt_name, size_t length, const int *value)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(opt_name);

        struct NXJOptionData *jdata = nx_json_options_find_in_map(jopt, opt_name);
        if (!jdata)
                nx_fatal(LOG_TAG, "Can not find option: %s", opt_name);

        if (jdata->type != NX_JOT_Int_Array)
                nx_fatal(LOG_TAG, "Error fetching option %s as int array", opt_name);

        nx_joption_data_realloc_value(jdata, length);
        for (size_t i = 0; i < length; ++i)
                jdata->value.ia[i] = value[i];
}

void nx_json_options_set_double_array(struct NXJSONOptions *jopt, const char *opt_name, size_t length, const double *value)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(opt_name);

        struct NXJOptionData *jdata = nx_json_options_find_in_map(jopt, opt_name);
        if (!jdata)
                nx_fatal(LOG_TAG, "Can not find option: %s", opt_name);

        if (jdata->type != NX_JOT_Double_Array)
                nx_fatal(LOG_TAG, "Error fetching option %s as double array", opt_name);

        nx_joption_data_realloc_value(jdata, length);
        for (size_t i = 0; i < length; ++i)
                jdata->value.da[i] = value[i];
}

void nx_json_options_set_string_array(struct NXJSONOptions *jopt, const char *opt_name, size_t length, char **value)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(opt_name);

        struct NXJOptionData *jdata = nx_json_options_find_in_map(jopt, opt_name);
        if (!jdata)
                nx_fatal(LOG_TAG, "Can not find option: %s", opt_name);

        if (jdata->type != NX_JOT_String_Array)
                nx_fatal(LOG_TAG, "Error fetching option %s as string array", opt_name);

        nx_joption_data_realloc_value(jdata, length);
        for (size_t i = 0; i < length; ++i)
                STRDUP(jdata->value.sa[i], value[i]);
}

NXBool *nx_json_options_get_bool_array(struct NXJSONOptions *jopt, const char *opt_name, size_t *length)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(opt_name);

        struct NXJOptionData *jdata = nx_json_options_find_in_map(jopt, opt_name);
        if (!jdata)
                nx_fatal(LOG_TAG, "Can not find option: %s", opt_name);

        if (jdata->type != NX_JOT_Bool_Array)
                nx_fatal(LOG_TAG, "Error fetching option %s as Boolean array", opt_name);

        *length = jdata->array_length;

        return jdata->value.ba;
}

int *nx_json_options_get_int_array(struct NXJSONOptions *jopt, const char *opt_name, size_t *length)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(opt_name);

        struct NXJOptionData *jdata = nx_json_options_find_in_map(jopt, opt_name);
        if (!jdata)
                nx_fatal(LOG_TAG, "Can not find option: %s", opt_name);

        if (jdata->type != NX_JOT_Int_Array)
                nx_fatal(LOG_TAG, "Error fetching option %s as int array", opt_name);

        *length = jdata->array_length;

        return jdata->value.ia;
}

double *nx_json_options_get_double_array(struct NXJSONOptions *jopt, const char *opt_name, size_t *length)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(opt_name);

        struct NXJOptionData *jdata = nx_json_options_find_in_map(jopt, opt_name);
        if (!jdata)
                nx_fatal(LOG_TAG, "Can not find option: %s", opt_name);

        if (jdata->type != NX_JOT_Double_Array)
                nx_fatal(LOG_TAG, "Error fetching option %s as double array", opt_name);

        *length = jdata->array_length;

        return jdata->value.da;
}

char **nx_json_options_get_string_array(struct NXJSONOptions *jopt, const char *opt_name, size_t *length)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(opt_name);

        struct NXJOptionData *jdata = nx_json_options_find_in_map(jopt, opt_name);
        if (!jdata)
                nx_fatal(LOG_TAG, "Can not find option: %s", opt_name);

        if (jdata->type != NX_JOT_String_Array)
                nx_fatal(LOG_TAG, "Error fetching option %s as string array", opt_name);

        *length = jdata->array_length;

        return jdata->value.sa;
}

NXBool nx_json_options_is_set(struct NXJSONOptions *jopt, const char *opt_name)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(opt_name);

        const struct NXJOptionData *jdata = nx_json_options_find_in_map(jopt, opt_name);
        if (!jdata)
                nx_fatal(LOG_TAG, "Can not find option: %s", opt_name);

        return jdata->is_set;
}

void nx_json_options_dump(const struct NXJSONOptions *jopt, FILE *stream)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(stream);

        int name_col_width = 0;
        const struct NXJOptionData *jdata = jopt->jopt_list.head;
        while (jdata != NULL) {
                int name_len = strlen(jdata->name);
                if (name_len > name_col_width)
                        name_col_width = name_len;
                jdata = jdata->next;
        }


        fprintf(stream, JOUT_BEGIN);
        jdata = jopt->jopt_list.head;
        while (jdata != NULL) {
                nx_joption_data_print(jdata, stream, name_col_width, jdata->next != NULL);
                jdata = jdata->next;
        }
        fprintf(stream, JOUT_END);
}

static inline char *get_token_string(const struct NXJSONReader *jreader, const jsmntok_t *t)
{
        size_t len = TOKEN_LENGTH(t);
        char *token_str = NX_NEW_C(len+1);

        const char*token_text = jreader->buffer + t->start;

        strncpy(token_str, token_text, len);
        token_str[len] = '\0';

        return token_str;
}

static inline char *get_option_name(const struct NXJSONReader *jreader, const jsmntok_t *t)
{
        char *token_str = get_token_string(jreader, t);

        if (t->type != JSMN_STRING)
                nx_fatal(LOG_TAG, "Expecting option name as string at position %d in the JSON stream, got %s!", t->start, token_str);

        return token_str;
}

void nx_json_options_parse_reader_tokens(struct NXJSONOptions *jopt, const struct NXJSONReader *jreader)
{
        const jsmntok_t *tokens = jreader->tokens;
        int n_tokens = jreader->n_tokens;

        if (n_tokens == 0)
                nx_fatal(LOG_TAG, "Empty JSON while trying to parse JSON options");

        const jsmntok_t *top_token = tokens + 0;
        if (top_token->type != JSMN_OBJECT)
                nx_fatal(LOG_TAG, "Top JSON field must be an object");

        int tid = 1;
        while (tid < jreader->n_tokens) {
                int name_tid = tid;
                const jsmntok_t *t = tokens + name_tid;
                char *opt_name = get_option_name(jreader, t);

                struct NXJOptionData *jdata = nx_json_options_find_in_map(jopt, opt_name);
                if (jdata == NULL) {
                        nx_warning(LOG_TAG, "Skipping unknown JSON option %s", opt_name);
                        int val_tid = ++tid;
                        tid += tokens[val_tid].size+1;
                } else {
                        int val_tid = ++tid;
                        tid = nx_joption_data_set_from_token(jdata, jreader, val_tid);
                }

                nx_free(opt_name);
        }
}

void nx_json_options_parse(struct NXJSONOptions *jopt, FILE *stream)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(stream);

        struct NXJSONReader *jreader = nx_json_reader_new();
        if (!nx_json_reader_parse_stream(jreader, stream))
                nx_fatal(LOG_TAG, "Error reading JSON options!");

        struct NXJOptionData *jdata = jopt->jopt_list.head;
        while (jdata != NULL) {
                jdata->is_set = NX_FALSE;
                jdata = jdata->next;
        }

        nx_json_options_parse_reader_tokens(jopt, jreader);

        jdata = jopt->jopt_list.head;
        while (jdata != NULL) {
                if (!jdata->is_set) {
                        if (jdata->is_required)
                                nx_fatal(LOG_TAG, "Missing required JSON option '%s'", jdata->name);
                        else
                                nx_joption_data_set_from_default_value(jdata);
                }
                jdata = jdata->next;
        }

        nx_json_reader_free(jreader);
}

void nx_json_options_save(const struct NXJSONOptions *jopt, const char *filename)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(filename);

        FILE *fout = nx_xfopen(filename, "w+");
        nx_json_options_dump(jopt, fout);
        nx_xfclose(fout, filename);
}

void nx_json_options_load(struct NXJSONOptions *jopt, const char *filename)
{
        NX_ASSERT_PTR(jopt);
        NX_ASSERT_PTR(filename);

        FILE *fin = nx_xfopen(filename, "r+");
        nx_json_options_parse(jopt, fin);
        nx_xfclose(fin, filename);
}

struct NXJOptionMapNode *nx_joption_map_node_new(const char *key, struct NXJOptionData *jdata)
{
        struct NXJOptionMapNode *node = NX_NEW(1, struct NXJOptionMapNode);
        node->key = NULL;
        STRDUP(node->key, key);
        node->data = jdata;
        node->next = NULL;

        return node;
}

void nx_joption_map_node_free(struct NXJOptionMapNode *jmnode)
{
        if (jmnode) {
                nx_free(jmnode->key);
                nx_free(jmnode);
        }
}

struct NXJOptionData *nx_joption_data_alloc()
{
        struct NXJOptionData *jdata = NX_NEW(1, struct NXJOptionData);
        memset(jdata, 0, sizeof(*jdata));
        return jdata;
}

struct NXJOptionData *nx_joption_data_new(const char *name, NXBool is_required)
{
        struct NXJOptionData *jdata = nx_joption_data_alloc();
        STRDUP(jdata->name, name);
        jdata->is_required = is_required;
        return jdata;
}

struct NXJOptionData *nx_joption_data_new_bool(const char *name, NXBool is_required, NXBool def_val)
{
        struct NXJOptionData *jdata = nx_joption_data_new(name, is_required);
        jdata->type = NX_JOT_Bool;
        jdata->value.b = def_val;
        jdata->default_value.b = def_val;
        return jdata;
}

struct NXJOptionData *nx_joption_data_new_int(const char *name, NXBool is_required, int def_val)
{
        struct NXJOptionData *jdata = nx_joption_data_new(name, is_required);
        jdata->type = NX_JOT_Int;
        jdata->value.i = def_val;
        jdata->default_value.i = def_val;
        return jdata;
}

struct NXJOptionData *nx_joption_data_new_double(const char *name, NXBool is_required, double def_val)
{
        struct NXJOptionData *jdata = nx_joption_data_new(name, is_required);
        jdata->type = NX_JOT_Double;
        jdata->value.d = def_val;
        jdata->default_value.d = def_val;
        return jdata;
}

struct NXJOptionData *nx_joption_data_new_string(const char *name, NXBool is_required, const char *def_val)
{
        struct NXJOptionData *jdata = nx_joption_data_new(name, is_required);
        jdata->type = NX_JOT_String;
        STRDUP(jdata->value.s, def_val);
        STRDUP(jdata->default_value.s, def_val);
        return jdata;
}

struct NXJOptionData *nx_joption_data_new_array(const char *name, enum NXJOptionType type, NXBool is_required)
{
        struct NXJOptionData *jdata = nx_joption_data_new(name, is_required);

        jdata->type = type;
        switch (type) {
        case NX_JOT_Bool_Array: jdata->value.ba = jdata->default_value.ba = NULL; break;
        case NX_JOT_Int_Array: jdata->value.ia = jdata->default_value.ia = NULL; break;
        case NX_JOT_Double_Array: jdata->value.da = jdata->default_value.da = NULL; break;
        case NX_JOT_String_Array: jdata->value.sa = jdata->default_value.sa = NULL; break;
        default:
                break;
        }

        return jdata;
}

void nx_joption_data_realloc_value(struct NXJOptionData *jdata, size_t n)
{
        nx_joption_data_free_value(jdata);

        switch (jdata->type) {
        case NX_JOT_Bool_Array: jdata->value.ba = NX_NEW(n, NXBool); break;
        case NX_JOT_Int_Array: jdata->value.ia = NX_NEW(n, int); break;
        case NX_JOT_Double_Array: jdata->value.da = NX_NEW(n, double); break;
        case NX_JOT_String_Array:
                jdata->value.sa = NX_NEW(n, char *);
                for (size_t i = 0; i < n; ++i)
                        jdata->value.sa[i] = NULL;
                break;
        default:
                break;
        }

        jdata->array_length = n;
}

void nx_joption_data_free_value(struct NXJOptionData *jdata)
{
        switch (jdata->type) {
        case NX_JOT_String:
                nx_free(jdata->value.s);
                jdata->value.s = NULL;
                break;
        case NX_JOT_Bool_Array:
                nx_free(jdata->value.ba);
                jdata->value.ba = NULL;
                break;
        case NX_JOT_Int_Array:
                nx_free(jdata->value.ia);
                jdata->value.ia = NULL;
                break;
        case NX_JOT_Double_Array:
                nx_free(jdata->value.da);
                jdata->value.da = NULL;
                break;
        case NX_JOT_String_Array:
                for (size_t i = 0; i < jdata->array_length; ++i)
                        nx_free(jdata->value.sa[i]);
                nx_free(jdata->value.sa);
                jdata->value.sa = NULL;
                break;
        default:
                break;
        }

        jdata->array_length = 0;
}

void nx_joption_data_free(struct NXJOptionData *jdata)
{
        if (jdata) {
                nx_free(jdata->name);
                nx_joption_data_free_value(jdata);
                if (jdata->type == NX_JOT_String)
                        nx_free(jdata->default_value.s);
                nx_free(jdata);
        }
}

void nx_joption_data_print_array_items(const struct NXJOptionData *jdata, FILE *stream)
{
        for (size_t i = 0; i < jdata->array_length; ++i) {
                switch (jdata->type) {
                case NX_JOT_Bool_Array: fprintf(stream, " %s", jdata->value.ba[i] ? "true" : "false"); break;
                case NX_JOT_Int_Array: fprintf(stream, " %d", jdata->value.ia[i]); break;
                case NX_JOT_Double_Array: fprintf(stream, " " JOUT_DOUBLE_FORMAT, jdata->value.da[i]); break;
                case NX_JOT_String_Array: fprintf(stream, " \"%s\"", jdata->value.sa[i]); break;
                default:
                        break;
                }
                fprintf(stream, "%s", (i < jdata->array_length-1) ? JOUT_ITEM_DELIMITER : "");
        }
}

void nx_joption_data_print(const struct NXJOptionData *jdata, FILE *stream, int name_col_width, NXBool has_next)
{
        int field_width = name_col_width - strlen(jdata->name) + 1;
        fprintf(stream, JOUT_INDENT_STRING "%*s%s\"" JOUT_NAME_DELIMITER,
                field_width, "\"", jdata->name);

        switch (jdata->type) {
        case NX_JOT_Bool: fprintf(stream, "%s", jdata->value.b ? "true" : "false"); break;
        case NX_JOT_Int: fprintf(stream, "%d", jdata->value.i); break;
        case NX_JOT_Double: fprintf(stream, JOUT_DOUBLE_FORMAT, jdata->value.d); break;
        case NX_JOT_String: fprintf(stream, "\"%s\"", jdata->value.s); break;
        case NX_JOT_Bool_Array:
        case NX_JOT_Int_Array:
        case NX_JOT_Double_Array:
        case NX_JOT_String_Array:
                fprintf(stream, JOUT_ARRAY_START);
                nx_joption_data_print_array_items(jdata, stream);
                fprintf(stream, JOUT_ARRAY_END);
        }

        if (has_next)
                fprintf(stream, JOUT_ITEM_DELIMITER);
        fprintf(stream, JOUT_NEWLINE);
}

static void parse_token_as_bool(const char *token_text, NXBool *is_bool, NXBool *value)
{
        *is_bool = NX_FALSE;
        *value = NX_FALSE;
        if (token_text[0] == 't' || token_text[0] == 'T') {
                *is_bool = NX_TRUE;
                *value = NX_TRUE;
        } else if (token_text[0] == 'f' || token_text[0] == 'F') {
                *is_bool = NX_TRUE;
                *value = NX_FALSE;
        }
}

void nx_joption_data_set_from_default_value(struct NXJOptionData *jdata)
{
        switch (jdata->type) {
        case NX_JOT_Bool: jdata->value.b = jdata->default_value.b; break;
        case NX_JOT_Int: jdata->value.i = jdata->default_value.i; break;
        case NX_JOT_Double: jdata->value.d = jdata->default_value.d; break;
        case NX_JOT_String: STRDUP(jdata->value.s, jdata->default_value.s); break;
        default:
                nx_joption_data_free_value(jdata);
                break;
        }
        jdata->is_set = NX_TRUE;
}

void nx_joption_data_set_from_primitive_token(struct NXJOptionData *jdata, const char *token_text)
{
        NXBool is_bool = NX_FALSE;
        NXBool bool_value = NX_FALSE;
        parse_token_as_bool(token_text, &is_bool, &bool_value);

        if (jdata->type == NX_JOT_Bool && !is_bool)
                nx_fatal(LOG_TAG, "Expecting Boolean value for JSON option %s", jdata->name);
        else if (jdata->type != NX_JOT_Bool && is_bool)
                nx_fatal(LOG_TAG, "Expecting numeric value for JSON option %s", jdata->name);

        switch (jdata->type) {
        case NX_JOT_Bool: jdata->value.b = bool_value; break;
        case NX_JOT_Int: jdata->value.i = atol(token_text); break;
        case NX_JOT_Double: jdata->value.d = atof(token_text); break;
        default: nx_fatal(LOG_TAG, "Invalid JSON option data to be set from primitive token");
        }
}

void nx_joption_data_set_array_element_from_primitive_token(struct NXJOptionData *jdata, int elem_id, const char *token_text)
{
        NXBool is_bool = NX_FALSE;
        NXBool bool_value = NX_FALSE;
        parse_token_as_bool(token_text, &is_bool, &bool_value);

        if (jdata->type == NX_JOT_Bool_Array && !is_bool)
                nx_fatal(LOG_TAG, "Expecting Boolean element for JSON option %s", jdata->name);
        else if (jdata->type != NX_JOT_Bool_Array && is_bool)
                nx_fatal(LOG_TAG, "Expecting numeric element for JSON option %s", jdata->name);

        switch (jdata->type) {
        case NX_JOT_Bool_Array: jdata->value.ba[elem_id] = bool_value; break;
        case NX_JOT_Int_Array: jdata->value.ia[elem_id] = atol(token_text); break;
        case NX_JOT_Double_Array: jdata->value.da[elem_id] = atof(token_text); break;
        default: nx_fatal(LOG_TAG, "Invalid JSON option data array element to be set from primitive token");
        }
}

void nx_joption_data_set_from_simple_token(struct NXJOptionData *jdata, const struct NXJSONReader *jreader, int token_id)
{
        const jsmntok_t *t_val = jreader->tokens + token_id;

        char *token_text = get_token_string(jreader, t_val);

        if (jdata->type == NX_JOT_String && t_val->type != JSMN_STRING)
                nx_fatal(LOG_TAG, "Expecting string value for JSON option %s", jdata->name);
        else if (jdata->type != NX_JOT_String && t_val->type != JSMN_PRIMITIVE)
                nx_fatal(LOG_TAG, "Expecting primitive value for JSON option %s", jdata->name);

        switch (jdata->type) {
        case NX_JOT_String: STRDUP(jdata->value.s, token_text); break;
        case NX_JOT_Bool:
        case NX_JOT_Int:
        case NX_JOT_Double:
                nx_joption_data_set_from_primitive_token(jdata, token_text);
                break;
        default:
                nx_fatal(LOG_TAG, "Invalid JSON option data to be set from simple token");
        }

        nx_free(token_text);
}

static void nx_joption_data_set_from_array(struct NXJOptionData *jdata, const struct NXJSONReader *jreader, int token_id)
{
        const jsmntok_t *t_val = jreader->tokens + token_id;

        if (t_val->type != JSMN_ARRAY)
                nx_fatal(LOG_TAG, "Expecting array value for JSON option %s", jdata->name);

        int n_elem = t_val->size;
        nx_joption_data_realloc_value(jdata, n_elem);

        for (int i = 0; i < n_elem; ++i) {
                const jsmntok_t *t_elem = t_val + i + 1;
                char *elem_text = get_token_string(jreader, t_elem);
                if (jdata->type == NX_JOT_String_Array && t_elem->type != JSMN_STRING)
                        nx_fatal(LOG_TAG, "Expecting string element for JSON option %s", jdata->name);
                else if (jdata->type != NX_JOT_String_Array && t_elem->type != JSMN_PRIMITIVE)
                        nx_fatal(LOG_TAG, "Expecting primitive element for JSON option %s", jdata->name);

                switch (jdata->type) {
                case NX_JOT_String_Array: STRDUP(jdata->value.sa[i], elem_text); break;
                case NX_JOT_Bool_Array:
                case NX_JOT_Int_Array:
                case NX_JOT_Double_Array:
                        nx_joption_data_set_array_element_from_primitive_token(jdata, i, elem_text);
                        break;
                default:
                        break;
                }

                nx_free(elem_text);
        }
}

int nx_joption_data_set_from_token(struct NXJOptionData *jdata, const struct NXJSONReader *jreader, int token_id)
{
        NXBool is_array = (jdata->type == NX_JOT_Bool_Array) || (jdata->type == NX_JOT_Int_Array)
                || (jdata->type == NX_JOT_Double_Array) || (jdata->type == NX_JOT_String_Array);

        int next_token_id;
        if (is_array) {
                nx_joption_data_set_from_array(jdata, jreader, token_id);

                const jsmntok_t *t_val = jreader->tokens + token_id;
                next_token_id = token_id + t_val->size + 1;
        } else {
                nx_joption_data_set_from_simple_token(jdata, jreader, token_id);
                next_token_id = token_id + 1;
        }

        jdata->is_set = NX_TRUE;
        return next_token_id;
}

struct NXJSONReader* nx_json_reader_new()
{
        struct NXJSONReader* reader = NX_NEW(1, struct NXJSONReader);

        reader->n_tokens = 0;
        reader->max_n_tokens = READER_INITIAL_N_TOKENS;
        reader->tokens = NX_NEW(READER_INITIAL_N_TOKENS, jsmntok_t);

        reader->buffer_size = READER_BUFFER_INITIAL_SIZE;
        reader->buffer = NX_NEW_C(READER_BUFFER_INITIAL_SIZE);

        jsmn_init(&reader->parser);

        return reader;
}

void nx_json_reader_free(struct NXJSONReader* reader)
{
        if (reader) {
                nx_free(reader->tokens);
                nx_free(reader->buffer);
                nx_free(reader);
        }
}

void nx_json_reader_read_stream(struct NXJSONReader* reader, FILE* stream)
{
        char line_buffer[4096];

        size_t sz = 0;
        while (NX_TRUE) {
                if (NULL == fgets(line_buffer, 4096, stream)) {
                        if (feof(stream))
                                break;
                        else
                                nx_fatal(LOG_TAG, "Error reading from JSON stream!");
                }

                size_t line_sz = strlen(line_buffer);
                sz += line_sz;
                if (sz > reader->buffer_size-1) {
                        reader->buffer_size = 2*sz + 1;
                        reader->buffer = (char *)nx_frealloc((void *)reader->buffer, reader->buffer_size);
                }
                memcpy(reader->buffer + sz - line_sz, line_buffer, line_sz);
        }

        reader->buffer[sz] = '\0';
}

void nx_json_reader_resize_tokens(struct NXJSONReader* reader)
{
        int new_size = reader->max_n_tokens * 2;
        reader->tokens = (jsmntok_t *)nx_frealloc((void *)reader->tokens, new_size*sizeof(jsmntok_t));
        reader->max_n_tokens = new_size;
}

NXBool nx_json_reader_parse_stream(struct NXJSONReader* reader, FILE* stream)
{
        reader->n_tokens = 0;

        nx_json_reader_read_stream(reader, stream);
        size_t buffer_sz = strlen(reader->buffer);

        NXBool parser_done = NX_FALSE;
        do {
                jsmn_init(&reader->parser);
                jsmnerr_t r = jsmn_parse(&reader->parser, reader->buffer, buffer_sz,
                                         reader->tokens, reader->max_n_tokens);
                if (r == JSMN_ERROR_NOMEM) {
                        nx_json_reader_resize_tokens(reader);
                } else if (r == JSMN_ERROR_PART) {
                        nx_error(LOG_TAG, "Incomplete JSON stream!");
                        return NX_FALSE;
                } else if (r == JSMN_ERROR_INVAL) {
                        nx_error(LOG_TAG, "Error parsing JSON stream!");
                        return NX_FALSE;
                } else {
                        reader->n_tokens = (int)r;
                        parser_done = NX_TRUE;
                }
        } while (!parser_done);

        return NX_TRUE;
}
