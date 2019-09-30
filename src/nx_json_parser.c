/**
 * @file nx_json_parser.c
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
#include "virg/nexus/nx_json_parser.h"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_log.h"

struct NXJSONParser
{
        struct NXJSONLexer *jlex;
        struct NXJSONToken *jtoken;

        struct NXJSONNode *root;
        struct NXJSONNode *current_node;
};

static void nx_json_parser_consume(struct NXJSONParser *jp)
{
        nx_json_lexer_next_token(jp->jlex, jp->jtoken);
}

struct NXJSONParser *nx_json_parser_new(struct NXJSONLexer *jlex)
{
        struct NXJSONParser *jp = NX_NEW(1, struct NXJSONParser);

        jp->jlex = jlex;
        jp->jtoken = NX_NEW(1, struct NXJSONToken);
        jp->jtoken->type = NX_JTT_INVALID;
        jp->jtoken->text = NULL;
        jp->root = NULL;
        jp->current_node = NULL;

        nx_json_parser_consume(jp);

        return jp;
}

void nx_json_parser_free(struct NXJSONParser *jp)
{
        if (jp != NULL) {
                nx_json_token_free_data(jp->jtoken);
                nx_free(jp->jtoken);
                nx_free(jp);
        }
}

static inline void nx_json_parser_add_child_node(struct NXJSONParser *jp, struct NXJSONNode *n)
{
        nx_json_node_add_child(jp->current_node, n);
}

static void nx_json_parser_match(struct NXJSONParser *jp, enum NXJSONTokenType type)
{
        if (jp->jtoken->type == type) {
                struct NXJSONNode *n = NULL;
                if (type == NX_JTT_STRING) {
                        n = nx_json_node_new_string(jp->jtoken->text);
                } else if (type == NX_JTT_INTEGER) {
                        n = nx_json_node_new_integer(jp->jtoken->text);
                } else if (type == NX_JTT_FPNUM) {
                        n = nx_json_node_new_fpnumber(jp->jtoken->text);
                } else if (type == NX_JTT_TRUE) {
                        n = nx_json_node_new_true();
                } else if (type == NX_JTT_FALSE) {
                        n = nx_json_node_new_false();
                } else if (type == NX_JTT_NULL) {
                        n = nx_json_node_new_null();
                }

                if (n != NULL)
                        nx_json_parser_add_child_node(jp, n);

                nx_json_parser_consume(jp);
        } else {
                NX_FATAL(NX_LOG_TAG, "Expecting %s, found %s", nx_json_token_type_name(type),
                         nx_json_token_name(jp->jtoken));
        }
}

static void nx_json_parser_parse_object(struct NXJSONParser *jp);
static void nx_json_parser_parse_array(struct NXJSONParser *jp);

static void nx_json_parser_parse_number(struct NXJSONParser *jp)
{
        if (jp->jtoken->type == NX_JTT_INTEGER)
                nx_json_parser_match(jp, NX_JTT_INTEGER);
        else if (jp->jtoken->type == NX_JTT_FPNUM)
                nx_json_parser_match(jp, NX_JTT_FPNUM);
        else
                NX_FATAL(NX_LOG_TAG, "Expecting number; found %s",
                         nx_json_token_to_string(jp->jtoken));
}

static void nx_json_parser_parse_value(struct NXJSONParser *jp)
{
        switch (jp->jtoken->type) {
        case NX_JTT_STRING: nx_json_parser_match(jp, NX_JTT_STRING); break;
        case NX_JTT_INTEGER:
        case NX_JTT_FPNUM: nx_json_parser_parse_number(jp); break;
        case NX_JTT_TRUE: nx_json_parser_match(jp, NX_JTT_TRUE); break;
        case NX_JTT_FALSE: nx_json_parser_match(jp, NX_JTT_FALSE); break;
        case NX_JTT_NULL: nx_json_parser_match(jp, NX_JTT_NULL); break;
        case NX_JTT_LSBRAC: nx_json_parser_parse_array(jp); break;
        case NX_JTT_LCBRAC: nx_json_parser_parse_object(jp); break;
        default:
                NX_FATAL(NX_LOG_TAG, "Expecting number, string, true, false, null, object or array, found %s",
                         nx_json_token_to_string(jp->jtoken));
        }
}

static void nx_json_parser_parse_elements(struct NXJSONParser *jp)
{
        nx_json_parser_parse_value(jp);

        while (jp->jtoken->type == NX_JTT_COMMA) {
                nx_json_parser_match(jp, NX_JTT_COMMA);
                nx_json_parser_parse_value(jp);
        }
}

void nx_json_parser_parse_array(struct NXJSONParser *jp)
{
        struct NXJSONNode *n = nx_json_node_new_array();
        if (jp->root == NULL) {
                jp->root = n;
                jp->current_node = n;
        } else {
                nx_json_parser_add_child_node(jp, n);
        }

        nx_json_parser_match(jp, NX_JTT_LSBRAC);

        if (jp->jtoken->type == NX_JTT_RSBRAC) {
                nx_json_parser_match(jp, NX_JTT_RSBRAC);
        } else {
                struct NXJSONNode *cn = jp->current_node;

                jp->current_node = n;
                nx_json_parser_parse_elements(jp);
                jp->current_node = cn;

                nx_json_parser_match(jp, NX_JTT_RSBRAC);
        }
}

static void nx_json_parser_parse_pair(struct NXJSONParser *jp)
{
        nx_json_parser_match(jp, NX_JTT_STRING);
        nx_json_parser_match(jp, NX_JTT_COLON);
        nx_json_parser_parse_value(jp);
}

static void nx_json_parser_parse_members(struct NXJSONParser *jp)
{
        nx_json_parser_parse_pair(jp);

        while (jp->jtoken->type == NX_JTT_COMMA) {
                nx_json_parser_match(jp, NX_JTT_COMMA);
                nx_json_parser_parse_pair(jp);
        }
}

void nx_json_parser_parse_object(struct NXJSONParser *jp)
{
        struct NXJSONNode *n = nx_json_node_new_object();
        if (jp->root == NULL) {
                jp->root = n;
                jp->current_node = n;
        } else {
                nx_json_parser_add_child_node(jp, n);
        }

        nx_json_parser_match(jp, NX_JTT_LCBRAC);

        if (jp->jtoken->type == NX_JTT_RCBRAC) {
                nx_json_parser_match(jp, NX_JTT_RCBRAC);
        } else {
                struct NXJSONNode *cn = jp->current_node;

                jp->current_node = n;
                nx_json_parser_parse_members(jp);
                jp->current_node = cn;

                nx_json_parser_match(jp, NX_JTT_RCBRAC);
        }
}

static void nx_json_parser_parse_json(struct NXJSONParser *jp)
{
        if (jp->jtoken->type == NX_JTT_LSBRAC) {
                nx_json_parser_parse_array(jp);
        } else if (jp->jtoken->type == NX_JTT_LCBRAC) {
                nx_json_parser_parse_object(jp);
        } else {
                NX_FATAL(NX_LOG_TAG, "Expecting array or object; found %s",
                         nx_json_token_to_string(jp->jtoken));
        }
}

struct NXJSONNode *nx_json_parser_parse(struct NXJSONParser *jp)
{
        jp->root = NULL;
        jp->current_node = NULL;
        nx_json_parser_parse_json(jp);
        return jp->root;
}
