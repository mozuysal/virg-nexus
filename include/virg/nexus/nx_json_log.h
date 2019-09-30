/**
 * @file nx_json_log.h
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
#ifndef VIRG_NEXUS_NX_JSON_LOG_H
#define VIRG_NEXUS_NX_JSON_LOG_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_io.h"
#include "virg/nexus/nx_json_tree.h"
#include "virg/nexus/nx_json_bundle.h"

__NX_BEGIN_DECL

#if defined(NX_ENABLE_JLOG)
#define NX_JLOG_N_STREAMS 32
#define NX_JLOG_PRETTY_PRINT_LEVEL 1

struct NXJSONNode *nx_json_log_get_stream(int jlog_id);
void nx_json_log_clear_all();
void nx_json_log_clear_stream(int jlog_id);
void nx_json_log_free();

#define NX_JLOG_CLEAR_ALL() nx_json_log_free()
#define NX_JLOG_CLEAR(jlogid) nx_json_log_clear_stream(jlogid)
#define NX_JLOG(jlogid,key,value) nx_json_object_add(nx_json_log_get_stream(jlogid), (key), (value))
#define NX_JLOG_TEMPLATE(jlogid,key,value,jbundlefn) nx_json_object_add(nx_json_log_get_stream(jlogid), (key), jbundlefn(value))
#define NX_JLOG_ARR_TEMPLATE(jlogid,key,n,value,jbundlefn) nx_json_object_add(nx_json_log_get_stream(jlogid), (key), jbundlefn((n),(value)))
#define NX_JLOG_ADD_TEMPLATE(jlogid,key,value,jbundlefn)                \
        do {                                                            \
                struct NXJSONNode *_nx_jlog_arr = nx_json_object_get(nx_json_log_get_stream(jlogid), (key), NX_JNT_ARRAY); \
                if (_nx_jlog_arr == NULL) {                             \
                        _nx_jlog_arr = nx_json_node_new_array();        \
                        NX_JLOG(jlogid,key,_nx_jlog_arr);               \
                }                                                       \
                nx_json_array_add(_nx_jlog_arr, jbundlefn(value));      \
        } while (0);

#define NX_JLOG_PRINT(jlogid) nx_json_tree_print(nx_json_log_get_stream(jlogid), NX_JLOG_PRETTY_PRINT_LEVEL)
#define NX_JLOG_FPRINT(jlogid,stream) nx_json_tree_fprint(stream,nx_json_log_get_stream(jlogid), NX_JLOG_PRETTY_PRINT_LEVEL)
#define NX_JLOG_XWRITE(jlogid,filename)                                 \
        do {                                                            \
                FILE *nx_jlog_stream = nx_xfopen(filename,"w");         \
                nx_json_tree_fprint(nx_jlog_stream, nx_json_log_get_stream(jlogid), NX_JLOG_PRETTY_PRINT_LEVEL); \
                nx_xfclose(nx_jlog_stream, filename);                   \
        } while (0);
#else
#  define NX_JLOG_CLEAR_ALL() do { (void)sizeof(int); } while(0)
#  define NX_JLOG_CLEAR(jlogid) do { (void)sizeof(jlogid); } while(0)
#  define NX_JLOG(jlogid,key,value) do { (void)sizeof(value); } while(0)
#  define NX_JLOG_TEMPLATE(jlogid,key,value,jbundlefn) do { (void)sizeof(value); } while(0)
#  define NX_JLOG_ARR_TEMPLATE(jlogid,key,n,value,jbundlefn) do { (void)sizeof(value); } while(0)
#  define NX_JLOG_ADD_TEMPLATE(jlogid,key,value,jbundlefn) do { (void)sizeof(value); } while(0)
#  define NX_JLOG_PRINT(jlogid) do { (void)sizeof(jlogid); } while(0)
#  define NX_JLOG_FPRINT(jlogid,stream) do { (void)sizeof(jlogid); } while(0)
#  define NX_JLOG_XWRITE(jlogid,filename) do { (void)sizeof(jlogid); } while(0)
#endif

#define NX_JLOG_I(jlogid,key,value) NX_JLOG_TEMPLATE(jlogid,key,value,nx_json_bundle_int)
#define NX_JLOG_F(jlogid,key,value) NX_JLOG_TEMPLATE(jlogid,key,value,nx_json_bundle_float)
#define NX_JLOG_D(jlogid,key,value) NX_JLOG_TEMPLATE(jlogid,key,value,nx_json_bundle_double)
#define NX_JLOG_S(jlogid,key,value) NX_JLOG_TEMPLATE(jlogid,key,value,nx_json_bundle_string)

#define NX_JLOG_IA(jlogid,key,n,value) NX_JLOG_ARR_TEMPLATE(jlogid,key,n,value,nx_json_bundle_int_array)
#define NX_JLOG_FA(jlogid,key,n,value) NX_JLOG_ARR_TEMPLATE(jlogid,key,n,value,nx_json_bundle_float_array)
#define NX_JLOG_DA(jlogid,key,n,value) NX_JLOG_ARR_TEMPLATE(jlogid,key,n,value,nx_json_bundle_double_array)
#define NX_JLOG_SA(jlogid,key,n,value) NX_JLOG_ARR_TEMPLATE(jlogid,key,n,value,nx_json_bundle_string_array)

#define NX_JLOG_ADDI(jlogid,key,value) NX_JLOG_ADD_TEMPLATE(jlogid,key,value,nx_json_bundle_int)
#define NX_JLOG_ADDF(jlogid,key,value) NX_JLOG_ADD_TEMPLATE(jlogid,key,value,nx_json_bundle_float)
#define NX_JLOG_ADDD(jlogid,key,value) NX_JLOG_ADD_TEMPLATE(jlogid,key,value,nx_json_bundle_double)
#define NX_JLOG_ADDS(jlogid,key,value) NX_JLOG_ADD_TEMPLATE(jlogid,key,value,nx_json_bundle_string)

__NX_END_DECL

#endif
