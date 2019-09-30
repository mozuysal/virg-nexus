/**
 * @file nx_options.c
 *
 * Copyright (C) 2019 Mustafa Ozuysal. All rights reserved.
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
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/nx_options.h"

#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_string.h"
#include "virg/nexus/nx_string_array.h"

#define OPTION_MAP_SIZE 32

enum NXOptionType {
        NX_OT_Bool,
        NX_OT_Int,
        NX_OT_Double,
        NX_OT_String,
        NX_OT_Rest,
        NX_OT_Help
};

union NXOptionValue {
        NXBool b;
        int i;
        double d;
        char *s;
        char **r;
};

struct NXOptionData {
        char *short_name;
        char *name;
        enum NXOptionType type;
        union NXOptionValue value;
        NXBool is_required;
        NXBool is_set;
        union NXOptionValue default_value;
        char *help;

        struct NXOptionData *next;
};

struct NXOptionMapNode {
        char *key;
        struct NXOptionData *data;
        struct NXOptionMapNode *next;
};

struct NXOptions {
        char *prog_name;

        int n_options;
        struct {
                struct NXOptionData *head;
                struct NXOptionData *tail;
        } opt_list;
        struct NXOptionMapNode **opt_map;

        struct NXOptionData *rest;

        char *usage_header;
        char *usage_footer;
};

static void nx_options_vadd(struct NXOptions *opt, const char *opt_format, va_list prm);
static void nx_options_print(const struct NXOptions *opt, FILE* stream);

static void nx_options_clear_set_flags(struct NXOptions *opt);

static void nx_options_add_bool(struct NXOptions *opt, const char *opt_names, const char *opt_help, NXBool default_value, NXBool is_required);
static void nx_options_add_int(struct NXOptions *opt, const char *opt_names, const char *opt_help, int default_value, NXBool is_required);
static void nx_options_add_double(struct NXOptions *opt, const char *opt_names, const char *opt_help, double default_value, NXBool is_required);
static void nx_options_add_string(struct NXOptions *opt, const char *opt_names, const char *opt_help, const char *default_value, NXBool is_required);
static void nx_options_add_rest(struct NXOptions *opt, const char *rest_name, NXBool is_required);

static void nx_options_free_list(struct NXOptions *opt);
static void nx_options_add_to_list(struct NXOptions *opt, struct NXOptionData *opt_data);

static void nx_options_init_map(struct NXOptions *opt);
static void nx_options_free_map(struct NXOptions *opt);
static void nx_options_add_to_map(struct NXOptions *opt, const char *key, struct NXOptionData *opt_data);
static int nx_options_hash_code(const char *key);
static struct NXOptionData *nx_options_find_in_map(const struct NXOptions *opt, const char *opt_name);

static struct NXOptionMapNode *nx_option_map_node_new(const char *key, struct NXOptionData *data);
static void nx_option_map_node_free(struct NXOptionMapNode *node);

static void nx_options_free_rest(struct NXOptions *opt);
static void nx_options_free_rest_data(struct NXOptionData *rest);

static struct NXOptionData *nx_option_data_new(const char *opt_names, const char *opt_help, NXBool is_required);
static void nx_option_data_free(struct NXOptionData *opt_data);
static void nx_option_data_add(struct NXOptionData *opt_data, struct NXOptions *opt);
static void nx_option_data_set_names(struct NXOptionData *opt_data, const char *opt_names);
static char *nx_option_data_default_value_string(const struct NXOptionData *opt_data);
static char *nx_option_data_value_string(const struct NXOptionData *opt_data);

static inline char *name_short_name_join(const char *name, const char *short_name);
static inline int compute_name_column_size(const struct NXOptions *opt, NXBool process_rest);

struct NXOptions *nx_options_alloc()
{
        struct NXOptions *opt = NX_NEW(1, struct NXOptions);
        opt->prog_name = NULL;
        opt->n_options = 0;
        opt->opt_list.head = NULL;
        opt->opt_list.tail = NULL;
        nx_options_init_map(opt);
        opt->rest = NULL;
        opt->usage_header = NULL;
        opt->usage_footer = NULL;

        return opt;
}

struct NXOptions *nx_options_new(const char *opt_format, ...)
{
        struct NXOptions *opt = nx_options_alloc();
        va_list prm;
        va_start(prm, opt_format);
        nx_options_vadd(opt, opt_format, prm);
        va_end(prm);
        return opt;
}

void nx_options_free(struct NXOptions *opt)
{
        if (opt) {
                nx_free(opt->prog_name);
                nx_options_free_list(opt);
                nx_options_free_map(opt);
                nx_options_free_rest(opt);
                nx_free(opt->usage_header);
                nx_free(opt->usage_footer);
                nx_free(opt);
        }
}

void nx_options_add(struct NXOptions *opt, const char *opt_format, ...)
{
        NX_ASSERT_PTR(opt);

        va_list prm;
        va_start(prm, opt_format);
        nx_options_vadd(opt, opt_format, prm);
        va_end(prm);
}

void nx_options_add_help(struct NXOptions *opt)
{
        NX_ASSERT_PTR(opt);

        struct NXOptionData *opt_data = nx_option_data_new("-h|--help", "Display usage help", NX_FALSE);
        opt_data->type = NX_OT_Help;

        nx_option_data_add(opt_data, opt);
}

void nx_options_vadd(struct NXOptions *opt, const char *opt_format, va_list prm)
{
        if (opt_format == NULL)
                return;

        int i = 0;
        char c;
        while ((c = opt_format[i++]) != '\0') {
                NXBool is_required = isupper(c);
                c = tolower(c);
                char *opt_names;
                char *opt_help;
                union NXOptionValue opt_val;
                switch (c) {
                case 'b':
                        opt_names = va_arg(prm, char *);
                        opt_help = va_arg(prm, char *);
                        opt_val.b = va_arg(prm, NXBool);
                        nx_options_add_bool(opt, opt_names, opt_help,
                                            opt_val.b, is_required);
                        break;
                case 'i':
                        opt_names = va_arg(prm, char *);
                        opt_help = va_arg(prm, char *);
                        opt_val.i = va_arg(prm, int);
                        nx_options_add_int(opt, opt_names, opt_help,
                                            opt_val.i, is_required);
                        break;
                case 'd':
                        opt_names = va_arg(prm, char *);
                        opt_help = va_arg(prm, char *);
                        opt_val.d = va_arg(prm, double);
                        nx_options_add_double(opt, opt_names, opt_help,
                                              opt_val.d, is_required);
                        break;
                case 's':
                        opt_names = va_arg(prm, char *);
                        opt_help = va_arg(prm, char *);
                        opt_val.s = va_arg(prm, char *);
                        nx_options_add_string(opt, opt_names, opt_help,
                                              opt_val.s, is_required);
                        break;
                case 'r':
                        opt_help = va_arg(prm, char *);
                        nx_options_add_rest(opt, opt_help, is_required);
                        break;
                default:
                        NX_FATAL(NX_LOG_TAG, "Unknown option character '%c'", c);
                        break;
                }
        }
}

static inline void nx_options_check_required(struct NXOptions *opt)
{
        struct NXOptionData *it = opt->opt_list.head;
        while (it != NULL) {
                if (!it->is_set) {
                        if (it->is_required) {
                                nx_options_print_usage(opt, stderr);
                                NX_FATAL(NX_LOG_TAG, "Missing required option '%s'!",
                                         name_short_name_join(it->name, it->short_name));
                        } else {
                                switch (it->type) {
                                case NX_OT_Bool: it->value.b = it->default_value.b; break;
                                case NX_OT_Int: it->value.i = it->default_value.i; break;
                                case NX_OT_Double: it->value.d = it->default_value.d; break;
                                case NX_OT_String: nx_strredup(&it->value.s, it->default_value.s); break;
                                default:
                                        break;
                                }
                        }
                }
                it = it->next;
        }

        if (opt->rest && !opt->rest->is_set) {
                if (opt->rest->is_required) {
                        NX_FATAL(NX_LOG_TAG, "Missing required argument <%s>!",
                                 opt->rest->help);
                } else {
                        nx_options_free_rest_data(opt->rest);
                }
        }
}

static void nx_options_set_rest_from_args(struct NXOptions *opt, int argc, char*argv[], const NXBool *is_processed)
{
        NX_ASSERT_PTR(opt->rest);

        int n_rest_arg = 0;
        for (int i = 0; i < argc; ++i) {
                if (!is_processed[i])
                        ++n_rest_arg;
        }

        nx_options_free_rest_data(opt->rest);
        opt->rest->value.r = NX_NEW(n_rest_arg+1, char *);
        opt->rest->default_value.i = n_rest_arg;

        for (int i = 0; i <= n_rest_arg; ++i)
                opt->rest->value.r[i] = NULL;

        int j = 0;
        for (int i = 0; i < argc; ++i) {
                if (!is_processed[i]) {
                        nx_strredup(&opt->rest->value.r[j], argv[i]);
                        ++j;
                }
        }

        if (n_rest_arg > 0)
                opt->rest->is_set = NX_TRUE;
}

void nx_options_set_from_args(struct NXOptions *opt, int argc, char **argv)
{
        NX_ASSERT_PTR(opt);

        nx_options_clear_set_flags(opt);

        NXBool *is_processed = NX_NEW(argc, NXBool);
        for (int i = 0; i < argc; ++i)
                is_processed[i] = NX_FALSE;

        nx_strredup(&opt->prog_name, argv[0]);
        is_processed[0] = NX_TRUE;

        for (int i = 1; i < argc; ++i) {
                struct NXOptionData *opt_data = nx_options_find_in_map(opt, argv[i]);
                if (opt_data == NULL)
                        continue;

                NXBool needs_param = opt_data->type == NX_OT_Int || opt_data->type == NX_OT_Double
                        || opt_data->type == NX_OT_String;

                char *arg_value;
                if (needs_param) {
                        if (i == argc-1)
                                NX_FATAL(NX_LOG_TAG, "Option %s expects a value following the option switch", argv[i]);
                        is_processed[i] = NX_TRUE;
                        arg_value = argv[++i];
                }

                switch (opt_data->type) {
                case NX_OT_Bool:
                        opt_data->value.b = NX_TRUE;
                        break;
                case NX_OT_Int:
                        opt_data->value.i = atol(arg_value);
                        break;
                case NX_OT_Double:
                        opt_data->value.d = atof(arg_value);
                        break;
                case NX_OT_String:
                        nx_strredup(&opt_data->value.s, arg_value);
                        break;
                case NX_OT_Rest:
                        break;
                case NX_OT_Help:
                        nx_options_print_usage(opt, stderr);
                        exit(EXIT_FAILURE);
                        break;
                }

                opt_data->is_set = NX_TRUE;
                is_processed[i] = NX_TRUE;
        }

        if (opt->rest != NULL) {
                nx_options_set_rest_from_args(opt, argc, argv, is_processed);
        } else {
                for (int i = 0; i < argc; ++i)
                        if (!is_processed[i])
                                NX_WARNING(NX_LOG_TAG, "Unused command-line argument '%s'", argv[i]);

        }

        nx_options_check_required(opt);

        nx_free(is_processed);
}

NXBool nx_options_is_set(const struct NXOptions *opt, const char *opt_name)
{
        NX_ASSERT_PTR(opt);

        struct NXOptionData *opt_data = nx_options_find_in_map(opt, opt_name);
        if (!opt_data)
                NX_FATAL(NX_LOG_TAG, "Can not find option: %s", opt_name);

        return opt_data->is_set;
}

NXBool nx_options_get_bool(const struct NXOptions *opt, const char *opt_name)
{
        NX_ASSERT_PTR(opt);

        struct NXOptionData *opt_data = nx_options_find_in_map(opt, opt_name);
        if (!opt_data)
                NX_FATAL(NX_LOG_TAG, "Can not find boolean option: %s", opt_name);

        if (opt_data->type != NX_OT_Bool)
                NX_FATAL(NX_LOG_TAG, "Error fetching option %s as boolean", opt_name);

        return opt_data->value.b;
}

int nx_options_get_int(const struct NXOptions *opt, const char *opt_name)
{
        NX_ASSERT_PTR(opt);

        struct NXOptionData *opt_data = nx_options_find_in_map(opt, opt_name);
        if (!opt_data)
                NX_FATAL(NX_LOG_TAG, "Can not find int option: %s", opt_name);

        if (opt_data->type != NX_OT_Int)
                NX_FATAL(NX_LOG_TAG, "Error fetching option %s as int", opt_name);

        return opt_data->value.i;
}

double nx_options_get_double(const struct NXOptions *opt, const char *opt_name)
{
        NX_ASSERT_PTR(opt);

        struct NXOptionData *opt_data = nx_options_find_in_map(opt, opt_name);
        if (!opt_data)
                NX_FATAL(NX_LOG_TAG, "Can not find double option: %s", opt_name);

        if (opt_data->type != NX_OT_Double)
                NX_FATAL(NX_LOG_TAG, "Error fetching option %s as double", opt_name);

        return opt_data->value.d;
}

const char *nx_options_get_string(const struct NXOptions *opt, const char *opt_name)
{
        NX_ASSERT_PTR(opt);

        struct NXOptionData *opt_data = nx_options_find_in_map(opt, opt_name);
        if (!opt_data)
                NX_FATAL(NX_LOG_TAG, "Can not find string option: %s", opt_name);

        if (opt_data->type != NX_OT_String)
                NX_FATAL(NX_LOG_TAG, "Error fetching option %s as string", opt_name);

        return opt_data->value.s;
}

char **nx_options_get_rest(const struct NXOptions *opt)
{
        NX_ASSERT_PTR(opt);

        struct NXOptionData *opt_data = opt->rest;
        if (!opt_data)
                return NULL;

        return opt_data->value.r;
}

void nx_options_print_values(const struct NXOptions *opt, FILE* stream)
{
        NX_ASSERT_PTR(opt);

        int name_col_width = compute_name_column_size(opt, NX_TRUE);
        const struct NXOptionData *it = opt->opt_list.head;
        while (it != NULL) {
                if (it->type != NX_OT_Help) {
                        char *val_str = nx_option_data_value_string(it);
                        char *name_join = name_short_name_join(it->name, it->short_name);
                        fprintf(stream, "%*s: %s\n", name_col_width,
                                name_join, val_str);
                        nx_free(val_str);
                        nx_free(name_join);
                }
                it = it->next;
        }

        if (opt->rest != NULL) {
                fprintf(stream, "%*s: [", name_col_width, opt->rest->help);
                size_t rest_len = opt->rest->default_value.i;
                for (int i = 0; i < rest_len; ++i) {
                        fprintf(stream, "'%s'", opt->rest->value.r[i]);
                        if (i != rest_len-1)
                                fprintf(stream, ", ");

                }
                fprintf(stream, "]\n");
        }
}

void nx_options_print_usage(const struct NXOptions *opt, FILE* stream)
{
        NX_ASSERT_PTR(opt);

        if (opt->usage_header != NULL)
                fprintf(stream, "%s", opt->usage_header);

        fprintf(stream, "USAGE: %s [options]", opt->prog_name != NULL ? opt->prog_name : "prog");
        if (opt->rest) {
                if (opt->rest->is_required) {
                        fprintf(stream, " <%s>", opt->rest->help);
                } else {
                        fprintf(stream, " [<%s>]", opt->rest->help);
                }
        }
        fprintf(stream, "\n");
        if (opt->opt_list.head != NULL) {
                nx_options_print(opt, stream);
        }

        if (opt->usage_footer != NULL)
                fprintf(stream, "%s", opt->usage_footer);

}

void nx_options_set_usage_header(struct NXOptions *opt, const char *header)
{
        NX_ASSERT_PTR(opt);

        nx_strredup(&opt->usage_header, header);
}

void nx_options_set_usage_footer(struct NXOptions *opt, const char *footer)
{
        NX_ASSERT_PTR(opt);

        nx_strredup(&opt->usage_footer, footer);
}

int compute_name_column_size(const struct NXOptions *opt, NXBool process_rest)
{
        struct NXOptionData *it = opt->opt_list.head;
        int name_col_width = 0;
        while (it != NULL) {
                int name_len = 0;
                if (it->name != NULL) {
                        name_len = strlen(it->name);
                        if (it->short_name != NULL)
                                name_len += strlen(it->short_name) + 2;
                } else {
                        name_len = strlen(it->short_name);
                }

                if (name_len > name_col_width)
                        name_col_width = name_len;
                it = it->next;
        }

        if (process_rest && opt->rest != NULL) {
                int rest_len = strlen(opt->rest->help);
                if (rest_len > name_col_width)
                        name_col_width = rest_len;
        }

        return name_col_width;
}

char *name_short_name_join(const char *name, const char *short_name)
{
        NXBool has_name = name != NULL;
        NXBool has_short_name = short_name != NULL;
        char *name_col = NULL;
        if (has_name && has_short_name) {
                size_t nsz = strlen(name);
                size_t snsz = strlen(short_name);
                size_t colsz = nsz+snsz+3;
                name_col = NX_NEW_C(colsz);
                strcpy(name_col, name);
                strcpy(name_col + nsz, ", ");
                strcpy(name_col + nsz + 2, short_name);
                name_col[colsz-1] = '\0';
        } else if (has_name) {
                nx_strredup(&name_col, name);
        } else {
                nx_strredup(&name_col, short_name);
        }

        return name_col;
}

void nx_options_print(const struct NXOptions *opt, FILE* stream)
{
        int name_col_width = compute_name_column_size(opt, NX_FALSE);
        struct NXOptionData *it = opt->opt_list.head;
        while (it != NULL) {
                char *name_col = name_short_name_join(it->name, it->short_name);
                char *default_value_str = nx_option_data_default_value_string(it);
                const char *open_delim = it->is_required ? "(" : (it->type != NX_OT_Help ? "[" : "");
                const char *close_delim = it->is_required ? ")" : (it->type != NX_OT_Help ? "]" : "");
                fprintf(stream, "  %*s : %s %s%s%s\n", name_col_width, name_col, it->help, open_delim,
                        it->is_required ? "REQUIRED" : default_value_str, close_delim);
                nx_free(name_col);
                nx_free(default_value_str);

                it = it->next;
        }
}

void nx_options_clear_set_flags(struct NXOptions *opt)
{
        struct NXOptionData *it = opt->opt_list.head;
        while (it != NULL) {
                it->is_set = NX_FALSE;
                it = it->next;
        }

        if (opt->rest != NULL)
                opt->rest->is_set = NX_FALSE;
}

void nx_options_add_bool(struct NXOptions *opt, const char *opt_names, const char *opt_help, NXBool default_value, NXBool is_required)
{
        struct NXOptionData *opt_data = nx_option_data_new(opt_names, opt_help, is_required);
        opt_data->type = NX_OT_Bool;
        opt_data->value.b = default_value;
        opt_data->default_value.b = default_value;

        nx_option_data_add(opt_data, opt);
}

void nx_options_add_int(struct NXOptions *opt, const char *opt_names, const char *opt_help, int default_value, NXBool is_required)
{
        struct NXOptionData *opt_data = nx_option_data_new(opt_names, opt_help, is_required);
        opt_data->type = NX_OT_Int;
        opt_data->value.i = default_value;
        opt_data->default_value.i = default_value;

        nx_option_data_add(opt_data, opt);
}

void nx_options_add_double(struct NXOptions *opt, const char *opt_names, const char *opt_help, double default_value, NXBool is_required)
{
        struct NXOptionData *opt_data = nx_option_data_new(opt_names, opt_help, is_required);
        opt_data->type = NX_OT_Double;
        opt_data->value.d = default_value;
        opt_data->default_value.d = default_value;

        nx_option_data_add(opt_data, opt);
}

void nx_options_add_string(struct NXOptions *opt, const char *opt_names, const char *opt_help, const char *default_value, NXBool is_required)
{
        struct NXOptionData *opt_data = nx_option_data_new(opt_names, opt_help, is_required);
        opt_data->type = NX_OT_String;
        nx_strredup(&opt_data->value.s, default_value);
        nx_strredup(&opt_data->default_value.s, default_value);

        nx_option_data_add(opt_data, opt);
}

void nx_options_add_rest(struct NXOptions *opt, const char *rest_name, NXBool is_required)
{
        struct NXOptionData *opt_data = nx_option_data_new("--", rest_name, is_required);
        opt_data->type = NX_OT_Rest;
        opt_data->value.r = NULL;
        opt_data->default_value.i = 0;

        nx_options_free_rest(opt);
        opt->rest = opt_data;
}

void nx_options_free_list(struct NXOptions *opt)
{
        NX_ASSERT_PTR(opt);

        while (opt->opt_list.head != NULL) {
                struct NXOptionData *opt_data = opt->opt_list.head;
                opt->opt_list.head = opt->opt_list.head->next;
                nx_option_data_free(opt_data);
        }
        opt->opt_list.tail = NULL;
}

void nx_options_add_to_list(struct NXOptions *opt, struct NXOptionData *opt_data)
{
        NX_ASSERT_PTR(opt);
        NX_ASSERT_PTR(opt_data);

        opt_data->next = NULL;
        if (opt->opt_list.head == NULL) {
                opt->opt_list.head = opt_data;
                opt->opt_list.tail = opt_data;
        } else {
                opt->opt_list.tail->next = opt_data;
                opt->opt_list.tail = opt_data;
        }
}

void nx_options_init_map(struct NXOptions *opt)
{
        NX_ASSERT_PTR(opt);

        opt->opt_map = NX_NEW(OPTION_MAP_SIZE, struct NXOptionMapNode *);
        for (int i = 0; i < OPTION_MAP_SIZE; ++i) {
                opt->opt_map[i] = NULL;
        }
}

void nx_options_add_to_map(struct NXOptions *opt, const char *key, struct NXOptionData *opt_data)
{
        NX_ASSERT_PTR(opt);
        NX_ASSERT_PTR(opt_data);

        int bid = abs(nx_options_hash_code(key) % OPTION_MAP_SIZE);

        /* fprintf(stderr, "Adding %s, (%s, %s) to map @ bucket %d\n", */
                /* key, opt_data->name, opt_data->short_name, bid); */

        struct NXOptionMapNode *node = nx_option_map_node_new(key, opt_data);
        node->next = opt->opt_map[bid];
        opt->opt_map[bid] = node;
}

struct NXOptionData *nx_options_find_in_map(const struct NXOptions *opt, const char *opt_name)
{
        NX_ASSERT_PTR(opt);

        if (opt_name == NULL)
                return NULL;

        int bid = abs(nx_options_hash_code(opt_name) % OPTION_MAP_SIZE);
        struct NXOptionMapNode *node = opt->opt_map[bid];
        while (node != NULL) {
                if (strcmp(node->key, opt_name) == 0)
                        return node->data;
                node = node->next;
        }

        return NULL;
}

void nx_options_free_map(struct NXOptions *opt)
{
        NX_ASSERT_PTR(opt);

        for (int i = 0; i < OPTION_MAP_SIZE; ++i) {
                while (opt->opt_map[i] != NULL) {
                        struct NXOptionMapNode *node = opt->opt_map[i];
                        opt->opt_map[i] = node->next;
                        nx_option_map_node_free(node);
                }
        }

        nx_free(opt->opt_map);
        opt->opt_map = NULL;
}

struct NXOptionMapNode *nx_option_map_node_new(const char *key, struct NXOptionData *data)
{
        struct NXOptionMapNode *node = NX_NEW(1, struct NXOptionMapNode);
        node->key = NULL;
        nx_strredup(&node->key, key);
        node->data = data;
        node->next = NULL;

        return node;
}

void nx_option_map_node_free(struct NXOptionMapNode *node)
{
        if (node) {
                nx_free(node->key);
                nx_free(node);
        }
}

int nx_options_hash_code(const char *key)
{
        NX_ASSERT_PTR(key);

        int hash = 0;
        while (*key != '\0') {
                hash = hash * 31 + *key;
                ++key;
        }
        return hash;
}

void nx_options_free_rest(struct NXOptions *opt)
{
        NX_ASSERT_PTR(opt);

        if (opt->rest) {
                nx_option_data_free(opt->rest);
                opt->rest = NULL;
        }
}

void nx_options_free_rest_data(struct NXOptionData *rest)
{
        if (rest && rest->value.r) {
                for (int i = 0; i < rest->default_value.i; ++i) {
                        nx_free(rest->value.r[i]);
                }
                nx_free(rest->value.r);
                rest->value.r = NULL;
                rest->default_value.i = 0;
        }
}

struct NXOptionData *nx_option_data_new(const char *opt_names, const char *opt_help, NXBool is_required)
{
        struct NXOptionData *opt_data = NX_NEW(1, struct NXOptionData);
        memset(opt_data, 0, sizeof(struct NXOptionData));
        nx_option_data_set_names(opt_data, opt_names);
        opt_data->is_required = is_required;
        opt_data->is_set = NX_FALSE;
        nx_strredup(&opt_data->help, opt_help);

        opt_data->next = NULL;

        return opt_data;
}

void nx_option_data_free(struct NXOptionData *opt_data)
{
        if (opt_data) {
                nx_free(opt_data->name);
                nx_free(opt_data->short_name);
                nx_free(opt_data->help);
                if (opt_data->type == NX_OT_String) {
                        nx_free(opt_data->value.s);
                        nx_free(opt_data->default_value.s);
                } else if (opt_data->type == NX_OT_Rest) {
                        nx_options_free_rest_data(opt_data);
                }

                nx_free(opt_data);
        }
}

void nx_option_data_add(struct NXOptionData *opt_data, struct NXOptions *opt)
{
        if (nx_options_find_in_map(opt, opt_data->name) != NULL
            || nx_options_find_in_map(opt, opt_data->short_name) != NULL)
                NX_FATAL(NX_LOG_TAG, "Can not add duplicate option (%s)",
                         name_short_name_join(opt_data->name, opt_data->short_name));

        nx_options_add_to_list(opt, opt_data);

        if (opt_data->name != NULL)
                nx_options_add_to_map(opt, opt_data->name, opt_data);

        if (opt_data->short_name != NULL)
                nx_options_add_to_map(opt, opt_data->short_name, opt_data);

        ++opt->n_options;
}

static char *nx_option_data_to_string(enum NXOptionType type, union NXOptionValue value)
{
        size_t MAX_BUFFER_SIZE = 1024;
        char *val_str = NX_NEW_C(MAX_BUFFER_SIZE);
        val_str[0] = '\0';

        switch (type) {
        case NX_OT_Bool:
                snprintf(val_str, MAX_BUFFER_SIZE, "%s", value.b ? "true" : "false");
                break;
        case NX_OT_Int:
                snprintf(val_str, MAX_BUFFER_SIZE, "%d", value.i);
                break;
        case NX_OT_Double:
                snprintf(val_str, MAX_BUFFER_SIZE, "%f", value.d);
                break;
        case NX_OT_String:
                snprintf(val_str, MAX_BUFFER_SIZE, "'%s'", value.s);
                break;
        default:
                break;
        }

        return val_str;
}

char *nx_option_data_default_value_string(const struct NXOptionData *opt_data)
{
        return nx_option_data_to_string(opt_data->type, opt_data->default_value);
}

char *nx_option_data_value_string(const struct NXOptionData *opt_data)
{
        return nx_option_data_to_string(opt_data->type, opt_data->value);
}

void nx_option_data_set_names(struct NXOptionData *opt_data, const char *opt_names)
{
        if (opt_names == NULL)
                NX_FATAL(NX_LOG_TAG, "Option names have to be non-NULL");

        char *names[2] = { NULL, NULL };

        const char *name_sep = strchr(opt_names, '|');
        if (name_sep == NULL) {
                nx_strredup(&names[0], opt_names);
        } else {
                int name0_len = strlen(opt_names) - strlen(name_sep);
                nx_strredup(&names[0], opt_names);
                names[0][name0_len] = '\0';
                name_sep++;
                nx_strredup(&names[1], name_sep);
        }

        for (int i = 0; i < 2; ++i) {
                if (names[i] == NULL)
                        continue;

                size_t l = strlen(names[i]);
                if (l < 2 || names[i][0] != '-')
                        NX_FATAL(NX_LOG_TAG, "Illegal option name \"%s\", option name "
                                 "must begin with -, and must include another character", names[i]);

                if (l > 2) {
                        if (names[i][1] != '-')
                                NX_FATAL(NX_LOG_TAG, "Illegal int option name \"%s\", int option names "
                                         "must begin with --", names[i]);
                        nx_strredup(&opt_data->name, names[i]);
                } else {
                        nx_strredup(&opt_data->short_name, names[i]);
                }

                nx_free(names[i]);
        }
}
