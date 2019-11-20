/**
 * @file nx_graph.c
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
 * along with VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/nx_graph.h"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_string.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_mat.h"

struct NXGraphAdjMat *nx_graph_new_adj_mat(const char *name, int n_v)
{
        NX_ASSERT(n_v >= 0);

        struct NXGraphAdjMat *g = NX_NEW(1, struct NXGraphAdjMat);
        g->name = nx_strdup(name ? name : "G");
        g->n_v = n_v;
        g->n_e = 0;
        g->weights = NX_NEW_D(n_v * n_v);
        nx_dmat_set(n_v, n_v, g->weights, n_v, NX_ABSENT_EDGE_WEIGHT);

        return g;
}

void nx_graph_free_adj_mat(struct NXGraphAdjMat *g)
{
        if (g) {
                nx_free(g->name);
                nx_free(g->weights);
                nx_free(g);
        }
}

void nx_graph_insert_adj_mat(struct NXGraphAdjMat *g,
                             int v, int w, double weight)
{
        NX_ASSERT_PTR(g);
        NX_ASSERT_INDEX(v, g->n_v);
        NX_ASSERT_INDEX(w, g->n_v);

        if (v == w) {
                NX_WARNING(NX_LOG_TAG, "Omitting insert edge to self in graph %s, %d -> %d", g->name, v, w);
                return;
        }

        if (g->weights[w * g->n_v + v] == NX_ABSENT_EDGE_WEIGHT)
                g->n_e++;
        g->weights[w * g->n_v + v] = weight;
        g->weights[v * g->n_v + w] = weight;
}

struct NXGraphEdge *nx_graph_edge_new(int v, double weight,
                                      struct NXGraphEdge *next)
{
        NX_ASSERT(v >= 0);

        struct NXGraphEdge *e = NX_NEW(1, struct NXGraphEdge);
        e->v = v;
        e->weight = weight;
        e->next = next;

        return e;
}

void nx_graph_edge_free(struct NXGraphEdge *e)
{
        nx_free(e);
}

void nx_graph_edge_list_free(struct NXGraphEdge *e)
{
        while (e) {
                struct NXGraphEdge *next = e->next;
                nx_graph_edge_free(e);
                e = next;
        }
}

struct NXGraphEdge *nx_graph_edge_list_seach(struct NXGraphEdge *head, int v)
{
        while (head != NULL) {
                if (head->v == v)
                        return head;
                head = head->next;
        }

        return NULL;
}

struct NXGraphAdjLst *nx_graph_new_adj_lst(const char *name, int n_v)
{
        NX_ASSERT(n_v >= 0);

        struct NXGraphAdjLst *g = NX_NEW(1, struct NXGraphAdjLst);
        g->name = nx_strdup(name ? name : "G");
        g->n_v = n_v;
        g->n_e = 0;
        g->edges = NX_NEW(n_v, struct NXGraphEdge *);

        for (int i = 0; i < n_v; ++i)
                g->edges[i] = NULL;

        return g;
}

void nx_graph_free_adj_lst(struct NXGraphAdjLst *g)
{
        if (g) {
                nx_free(g->name);
                for (int i = 0; i < g->n_v; ++i)
                        nx_graph_edge_list_free(g->edges[i]);
                nx_free(g->edges);
                nx_free(g);
        }
}

void nx_graph_insert_adj_lst(struct NXGraphAdjLst *g,
                             int v, int w, double weight)
{
        NX_ASSERT_PTR(g);
        NX_ASSERT_INDEX(v, g->n_v);
        NX_ASSERT_INDEX(w, g->n_v);

        if (v == w) {
                NX_WARNING(NX_LOG_TAG, "Omitting insert edge to self in graph %s, %d -> %d", g->name, v, w);
                return;
        }

        struct NXGraphEdge *e = nx_graph_edge_list_seach(g->edges[v], w);
        if (e)
                e->weight = weight;
        else
                g->edges[v] = nx_graph_edge_new(w, weight, g->edges[v]);

        e = nx_graph_edge_list_seach(g->edges[w], v);
        if (e)
                e->weight = weight;
        else
                g->edges[w] = nx_graph_edge_new(v, weight, g->edges[w]);
}
