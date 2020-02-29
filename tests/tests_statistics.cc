/**
 * @file tests_statistics.cc
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

#include "gtest/gtest.h"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_statistics.h"

using namespace std;

namespace {

static const int MAX_LEN = 5;

class NXStatisticsTest : public ::testing::Test {
protected:
        NXStatisticsTest() {
        }

        virtual void SetUp() {
                v = NX_NEW_D(MAX_LEN);
                vi = NX_NEW_I(MAX_LEN);
                for (int i = 0; i < MAX_LEN; ++i) {
                        v[i] = i + 1.0;
                        vi[i] = i + 1;
                }
        }

        virtual void TearDown() {
                nx_free(v);
                nx_free(vi);
        }

        struct NXStatistics1D stats_1d;
        double *v;
        int *vi;
};

TEST_F(NXStatisticsTest, one_dim_0) {
        nx_dstatistics_1d(&stats_1d, 0, &v[0]);
        EXPECT_EQ(0, stats_1d.n);
        EXPECT_EQ(0.0, stats_1d.mean);
        EXPECT_EQ(0.0, stats_1d.median);
        EXPECT_EQ(0.0, stats_1d.lower_quartile);
        EXPECT_EQ(0.0, stats_1d.upper_quartile);
        EXPECT_EQ(0.0, stats_1d.interquartile_range);
        EXPECT_EQ(0.0, stats_1d.stdev);
        EXPECT_EQ(0.0, stats_1d.min);
        EXPECT_EQ(0.0, stats_1d.max);

        nx_istatistics_1d(&stats_1d, 0, &vi[0]);
        EXPECT_EQ(0, stats_1d.n);
        EXPECT_EQ(0.0, stats_1d.mean);
        EXPECT_EQ(0.0, stats_1d.median);
        EXPECT_EQ(0.0, stats_1d.lower_quartile);
        EXPECT_EQ(0.0, stats_1d.upper_quartile);
        EXPECT_EQ(0.0, stats_1d.interquartile_range);
        EXPECT_EQ(0.0, stats_1d.stdev);
        EXPECT_EQ(0.0, stats_1d.min);
        EXPECT_EQ(0.0, stats_1d.max);
}

TEST_F(NXStatisticsTest, one_dim_1) {
        nx_dstatistics_1d(&stats_1d, 1, &v[0]);
        EXPECT_EQ(1, stats_1d.n);
        EXPECT_EQ(1.0, stats_1d.mean);
        EXPECT_EQ(1.0, stats_1d.median);
        EXPECT_EQ(1.0, stats_1d.lower_quartile);
        EXPECT_EQ(1.0, stats_1d.upper_quartile);
        EXPECT_EQ(0.0, stats_1d.interquartile_range);
        EXPECT_EQ(0.0, stats_1d.stdev);
        EXPECT_EQ(1.0, stats_1d.min);
        EXPECT_EQ(1.0, stats_1d.max);

        nx_istatistics_1d(&stats_1d, 1, &vi[0]);
        EXPECT_EQ(1, stats_1d.n);
        EXPECT_EQ(1.0, stats_1d.mean);
        EXPECT_EQ(1.0, stats_1d.median);
        EXPECT_EQ(1.0, stats_1d.lower_quartile);
        EXPECT_EQ(1.0, stats_1d.upper_quartile);
        EXPECT_EQ(0.0, stats_1d.interquartile_range);
        EXPECT_EQ(0.0, stats_1d.stdev);
        EXPECT_EQ(1.0, stats_1d.min);
        EXPECT_EQ(1.0, stats_1d.max);
}

TEST_F(NXStatisticsTest, one_dim_2) {
        nx_dstatistics_1d(&stats_1d, 2, &v[0]);
        EXPECT_EQ(2, stats_1d.n);
        EXPECT_EQ(1.5, stats_1d.mean);
        EXPECT_EQ(1.5, stats_1d.median);
        EXPECT_EQ(1.0, stats_1d.lower_quartile);
        EXPECT_EQ(2.0, stats_1d.upper_quartile);
        EXPECT_EQ(1.0, stats_1d.interquartile_range);
        EXPECT_EQ(sqrt(0.5), stats_1d.stdev);
        EXPECT_EQ(1.0, stats_1d.min);
        EXPECT_EQ(2.0, stats_1d.max);

        nx_istatistics_1d(&stats_1d, 2, &vi[0]);
        EXPECT_EQ(2, stats_1d.n);
        EXPECT_EQ(1.5, stats_1d.mean);
        EXPECT_EQ(1.5, stats_1d.median);
        EXPECT_EQ(1.0, stats_1d.lower_quartile);
        EXPECT_EQ(2.0, stats_1d.upper_quartile);
        EXPECT_EQ(1.0, stats_1d.interquartile_range);
        EXPECT_EQ(sqrt(0.5), stats_1d.stdev);
        EXPECT_EQ(1.0, stats_1d.min);
        EXPECT_EQ(2.0, stats_1d.max);
}

TEST_F(NXStatisticsTest, one_dim_3) {
        nx_dstatistics_1d(&stats_1d, 3, &v[0]);
        EXPECT_EQ(3, stats_1d.n);
        EXPECT_EQ(2.0, stats_1d.mean);
        EXPECT_EQ(2.0, stats_1d.median);
        EXPECT_EQ(1.0, stats_1d.lower_quartile);
        EXPECT_EQ(3.0, stats_1d.upper_quartile);
        EXPECT_EQ(2.0, stats_1d.interquartile_range);
        EXPECT_EQ(1.0, stats_1d.stdev);
        EXPECT_EQ(1.0, stats_1d.min);
        EXPECT_EQ(3.0, stats_1d.max);

        nx_istatistics_1d(&stats_1d, 3, &vi[0]);
        EXPECT_EQ(3, stats_1d.n);
        EXPECT_EQ(2.0, stats_1d.mean);
        EXPECT_EQ(2.0, stats_1d.median);
        EXPECT_EQ(1.0, stats_1d.lower_quartile);
        EXPECT_EQ(3.0, stats_1d.upper_quartile);
        EXPECT_EQ(2.0, stats_1d.interquartile_range);
        EXPECT_EQ(1.0, stats_1d.stdev);
        EXPECT_EQ(1.0, stats_1d.min);
        EXPECT_EQ(3.0, stats_1d.max);
}

TEST_F(NXStatisticsTest, one_dim_4) {
        nx_dstatistics_1d(&stats_1d, 4, &v[0]);
        EXPECT_EQ(4, stats_1d.n);
        EXPECT_EQ(2.5, stats_1d.mean);
        EXPECT_EQ(2.5, stats_1d.median);
        EXPECT_EQ(1.5, stats_1d.lower_quartile);
        EXPECT_EQ(3.5, stats_1d.upper_quartile);
        EXPECT_EQ(2.0, stats_1d.interquartile_range);
        EXPECT_EQ(sqrt(5.0 / 3.0), stats_1d.stdev);
        EXPECT_EQ(1.0, stats_1d.min);
        EXPECT_EQ(4.0, stats_1d.max);

        nx_istatistics_1d(&stats_1d, 4, &vi[0]);
        EXPECT_EQ(4, stats_1d.n);
        EXPECT_EQ(2.5, stats_1d.mean);
        EXPECT_EQ(2.5, stats_1d.median);
        EXPECT_EQ(1.5, stats_1d.lower_quartile);
        EXPECT_EQ(3.5, stats_1d.upper_quartile);
        EXPECT_EQ(2.0, stats_1d.interquartile_range);
        EXPECT_EQ(sqrt(5.0 / 3.0), stats_1d.stdev);
        EXPECT_EQ(1.0, stats_1d.min);
        EXPECT_EQ(4.0, stats_1d.max);
}

TEST_F(NXStatisticsTest, one_dim_5) {
        nx_dstatistics_1d(&stats_1d, 5, &v[0]);
        EXPECT_EQ(5, stats_1d.n);
        EXPECT_EQ(3.0, stats_1d.mean);
        EXPECT_EQ(3.0, stats_1d.median);
        EXPECT_EQ(1.5, stats_1d.lower_quartile);
        EXPECT_EQ(4.5, stats_1d.upper_quartile);
        EXPECT_EQ(3.0, stats_1d.interquartile_range);
        EXPECT_EQ(sqrt(10.0 / 4.0), stats_1d.stdev);
        EXPECT_EQ(1.0, stats_1d.min);
        EXPECT_EQ(5.0, stats_1d.max);

        nx_istatistics_1d(&stats_1d, 5, &vi[0]);
        EXPECT_EQ(5, stats_1d.n);
        EXPECT_EQ(3.0, stats_1d.mean);
        EXPECT_EQ(3.0, stats_1d.median);
        EXPECT_EQ(1.5, stats_1d.lower_quartile);
        EXPECT_EQ(4.5, stats_1d.upper_quartile);
        EXPECT_EQ(3.0, stats_1d.interquartile_range);
        EXPECT_EQ(sqrt(10.0 / 4.0), stats_1d.stdev);
        EXPECT_EQ(1.0, stats_1d.min);
        EXPECT_EQ(5.0, stats_1d.max);
}

} // namespace
