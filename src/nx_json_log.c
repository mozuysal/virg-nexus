/**
 * @file nx_json_log.c
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
#define NX_ENABLE_JLOG 1
#include "virg/nexus/nx_json_log.h"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_log.h"

static struct NXJSONNode **nx_s_json_log = NULL;

struct NXJSONNode *nx_json_log_get_stream(int jlog_id)
{
        if (nx_s_json_log == NULL) {
                nx_s_json_log = NX_NEW(NX_JLOG_N_STREAMS, struct NXJSONNode *);
                for (int i = 0; i < NX_JLOG_N_STREAMS; ++i)
                        nx_s_json_log[i] = NULL;
        }

        if (jlog_id < 0 || jlog_id >= NX_JLOG_N_STREAMS)
                NX_FATAL(NX_LOG_TAG, "JSON log id %d is not in range [0, %d]",
                         jlog_id, NX_JLOG_N_STREAMS-1);
        else if (nx_s_json_log[jlog_id] == NULL)
                nx_s_json_log[jlog_id] = nx_json_node_new_object();

        return nx_s_json_log[jlog_id];
}

void nx_json_log_free()
{
        if (nx_s_json_log != NULL) {
                for (int i = 0; i < NX_JLOG_N_STREAMS; ++i)
                        nx_json_tree_free(nx_s_json_log[i]);
                nx_free(nx_s_json_log);
                nx_s_json_log = NULL;
        }
}

void nx_json_log_clear_stream(int jlog_id)
{
        if (jlog_id < 0 || jlog_id >= NX_JLOG_N_STREAMS)
                NX_FATAL(NX_LOG_TAG, "JSON log id %d is not in range [0, %d]",
                         jlog_id, NX_JLOG_N_STREAMS-1);
        else if (nx_s_json_log == NULL || nx_s_json_log[jlog_id] == NULL)
                return;

        nx_json_tree_free(nx_s_json_log[jlog_id]);
        nx_s_json_log[jlog_id] = NULL;
}
