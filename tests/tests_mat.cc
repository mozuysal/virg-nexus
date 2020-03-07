/**
 * @file tests_mat.cc
 *
 * Copyright (C) 2020 Mustafa Ozuysal. All rights reserved.
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
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <ctime>
#include <cinttypes>

#include "gtest/gtest.h"

#include "virg/nexus/nx_mat.h"
#include "virg/nexus/nx_vec.h"
#include "virg/nexus/nx_io.h"
#include "virg/nexus/nx_alloc.h"

using namespace std;

namespace {

class NXMatTest : public ::testing::Test {
protected:
        NXMatTest() {
        }

        virtual void SetUp() {
        }

        virtual void TearDown() {
        }
};

TEST_F(NXMatTest, transpose) {
        double A[] = { 0.0, 1.0, 2.0,
                       3.0, 4.0, 5.0,
                       6.0, 7.0, 8.0,
                       9.0, 10.0, 11.0 };
        double *At = NX_NEW_D(12);

        nx_dvec_copy(12, At, &A[0]);
        nx_dmat_transpose(3, 4, At);

        EXPECT_EQ(0.0, At[0]);
        EXPECT_EQ(3.0, At[1]);
        EXPECT_EQ(6.0, At[2]);
        EXPECT_EQ(9.0, At[3]);
        EXPECT_EQ(1.0, At[4]);
        EXPECT_EQ(4.0, At[5]);
        EXPECT_EQ(7.0, At[6]);
        EXPECT_EQ(10.0, At[7]);
        EXPECT_EQ(2.0, At[8]);
        EXPECT_EQ(5.0, At[9]);
        EXPECT_EQ(8.0, At[10]);
        EXPECT_EQ(11.0, At[11]);

        nx_dvec_copy(12, At, &A[0]);
        nx_dmat_transpose(2, 6, At);

        EXPECT_EQ(0.0, At[0]);
        EXPECT_EQ(2.0, At[1]);
        EXPECT_EQ(4.0, At[2]);
        EXPECT_EQ(6.0, At[3]);
        EXPECT_EQ(8.0, At[4]);
        EXPECT_EQ(10.0, At[5]);
        EXPECT_EQ(1.0, At[6]);
        EXPECT_EQ(3.0, At[7]);
        EXPECT_EQ(5.0, At[8]);
        EXPECT_EQ(7.0, At[9]);
        EXPECT_EQ(9.0, At[10]);
        EXPECT_EQ(11.0, At[11]);

        nx_free(At);
}

TEST_F(NXMatTest, read_write) {
        double A[] = { 0.0, 1.0, 2.0,
                       10.0 / 3.0, 4.0, 5.0,
                       6.0, 7.0, 8.0,
                       9.4, 10.0, 11.0 };

        FILE *stream = nx_xfopen("/tmp/A.txt", "w");
        nx_dmat_xwrite(stream, 3, 4, &A[0]);
        nx_xfclose(stream, "A");

        stream = nx_xfopen("/tmp/A.txt", "r");
        int m;
        int n;
        double *Ar = nx_dmat_xread(stream, &m, &n);
        nx_xfclose(stream, "A");

        EXPECT_EQ(3, m);
        EXPECT_EQ(4, n);
        for (int i = 0; i < 12; ++i)
                EXPECT_EQ(A[i], Ar[i]);

        nx_free(Ar);
}

TEST_F(NXMatTest, read_fail) {
        FILE *stream = nx_xfopen("/tmp/A_death.txt", "w");
        fprintf(stream, "1.0 2.0 3.0\n");
        fprintf(stream, "1.0 2.0 3.0\n");
        fprintf(stream, "1.0 2.0\n");
        fprintf(stream, "1.0 2.0 3.0\n");
        nx_xfclose(stream, "A");

        stream = nx_xfopen("/tmp/A_death.txt", "r");
        int m;
        int n;
        EXPECT_DEATH(nx_dmat_xread(stream, &m, &n), "Reading matrix failed: Expected 3 elements on line 3, found 2!");
}

} // namespace
