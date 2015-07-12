/**
 * @file nx_json_log.c
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
#define NX_ENABLE_JLOG 1
#include "virg/nexus/nx_json_log.h"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_message.h"

static struct NXJSONNode **nx_s_json_log = NULL;

struct NXJSONNode *nx_json_log_get_stream(int jlog_id)
{
        if (nx_s_json_log == NULL) {
                nx_s_json_log = NX_NEW(NX_JLOG_N_STREAMS, struct NXJSONNode *);
                for (int i = 0; i < NX_JLOG_N_STREAMS; ++i)
                        nx_s_json_log[i] = NULL;
        }

        if (jlog_id < 0 || jlog_id >= NX_JLOG_N_STREAMS)
                nx_fatal(NX_LOG_TAG, "JSON log id %d is not in range [0, %d]",
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
                nx_fatal(NX_LOG_TAG, "JSON log id %d is not in range [0, %d]",
                         jlog_id, NX_JLOG_N_STREAMS-1);
        else if (nx_s_json_log == NULL || nx_s_json_log[jlog_id] == NULL)
                return;

        nx_json_tree_free(nx_s_json_log[jlog_id]);
        nx_s_json_log[jlog_id] = NULL;
}
