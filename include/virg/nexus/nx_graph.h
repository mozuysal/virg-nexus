/**
 * @file nx_graph.h
 *
 * Functions and data structures for dealing with undirected graphs.
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
 * VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_GRAPH_H
#define VIRG_NEXUS_NX_GRAPH_H

#include <float.h>

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

#define NX_ABSENT_EDGE_WEIGHT DBL_MAX

struct NXGraphAdjMat {
        char *name;
        int n_v;
        int n_e;
        double *weights;
};

struct NXGraphAdjMat *nx_graph_new_adj_mat(const char *name, int n_v);
void nx_graph_free_adj_mat(struct NXGraphAdjMat *g);

void nx_graph_insert_adj_mat(struct NXGraphAdjMat *g,
                             int v, int w, double weight);

struct NXGraphEdge {
        int v;
        double weight;
        struct NXGraphEdge *next;
};

struct NXGraphEdge *nx_graph_edge_new(int v, double weight,
                                      struct NXGraphEdge *next);
void nx_graph_edge_free(struct NXGraphEdge *e);
void nx_graph_edge_list_free(struct NXGraphEdge *e);
struct NXGraphEdge *nx_graph_edge_list_seach(struct NXGraphEdge *head, int v);

struct NXGraphAdjLst {
        char *name;
        int n_v;
        int n_e;
        struct NXGraphEdge **edges;
};

struct NXGraphAdjLst *nx_graph_new_adj_lst(const char *name, int n_v);
void nx_graph_free_adj_lst(struct NXGraphAdjLst *g);

void nx_graph_insert_adj_lst(struct NXGraphAdjLst *g,
                             int v, int w, double weight);

__NX_END_DECL

#endif
