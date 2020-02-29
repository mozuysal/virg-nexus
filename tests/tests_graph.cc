/**
 * @file tests_graph.cc
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
 * You should have received a copy of the GNU General Public License
 * along with VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <ctime>

#include "gtest/gtest.h"

#include "virg/nexus/nx_graph.h"

using namespace std;

static const int NV = 10;

namespace {

class NXGraphTest : public ::testing::Test {
protected:
        NXGraphTest()
                {
                }

        virtual void SetUp()
                {
                }

        virtual void TearDown()
                {
                }
};

TEST_F(NXGraphTest, new_adj_mat) {
        struct NXGraphAdjMat *g = nx_graph_new_adj_mat(NULL, NV);
        EXPECT_EQ(NV, g->n_v);
        EXPECT_EQ(0, g->n_e);
        for (int i = 0; i < g->n_v; ++i)
                for (int j = 0; j < g->n_v; ++j)
                        EXPECT_EQ(NX_ABSENT_EDGE_WEIGHT,\
                                  g->weights[i*g->n_v + j]);
        nx_graph_free_adj_mat(g);
}

TEST_F(NXGraphTest, fully_connected_adj_mat) {
        struct NXGraphAdjMat *g = nx_graph_new_adj_mat(NULL, NV);
        for (int i = 0; i < g->n_v; ++i)
                for (int j = 0; j < g->n_v; ++j)
                        nx_graph_insert_adj_mat(g, i, j, 1.0);
        for (int i = 0; i < g->n_v; ++i)
                for (int j = 0; j < g->n_v; ++j)
                        if (i == j)
                                EXPECT_EQ(NX_ABSENT_EDGE_WEIGHT,        \
                                          g->weights[i*g->n_v + j]);
                        else
                                EXPECT_DOUBLE_EQ(1.0, g->weights[i*g->n_v + j]);
        nx_graph_free_adj_mat(g);
}


TEST_F(NXGraphTest, new_adj_lst) {
        struct NXGraphAdjLst *g = nx_graph_new_adj_lst(NULL, NV);
        EXPECT_EQ(NV, g->n_v);
        EXPECT_EQ(0, g->n_e);
        for (int i = 0; i < g->n_v; ++i)
                EXPECT_EQ(NULL, g->edges[i]);
        nx_graph_free_adj_lst(g);
}

TEST_F(NXGraphTest, fully_connected_adj_lst) {
        struct NXGraphAdjLst *g = nx_graph_new_adj_lst(NULL, NV);
        for (int i = 0; i < g->n_v; ++i)
                for (int j = 0; j < g->n_v; ++j)
                        nx_graph_insert_adj_lst(g, i, j, 1.0);
        for (int i = 0; i < g->n_v; ++i) {
                for (int j = 0; j < g->n_v; ++j) {
                        struct NXGraphEdge *e = nx_graph_edge_list_seach(g->edges[i], j);
                        if (i == j)
                                EXPECT_EQ(NULL, e);
                        else
                                EXPECT_DOUBLE_EQ(1.0, e->weight);
                }
        }
        nx_graph_free_adj_lst(g);
}

} // namespace
