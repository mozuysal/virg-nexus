/**
 * @file nx_statistics.c
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
#include "virg/nexus/nx_statistics.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <limits.h>

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_math.h"

void nx_dstatistics_1d(struct NXStatistics1D *stats, int n, double *x)
{
        NX_ASSERT_PTR(stats);
        NX_ASSERT((x && n > 0) || (n == 0));

        memset(stats, 0, sizeof(*stats));
        stats->n = n;

        if (n == 0) {
                return;
        } else if (n == 1) {
                stats->mean = x[0];
                stats->median = x[0];
                stats->lower_quartile = x[0];
                stats->upper_quartile = x[0];
                stats->interquartile_range = 0.0;
                stats->stdev = 0.0;
                stats->min = x[0];
                stats->max = x[0];
                return;
        }

        qsort((void *)x, n, sizeof(double), nx_compare_double_incr);
        int half_n = n / 2;
        int half_half_n = half_n / 2;
        if (n & 1) {
                stats->median = x[half_n];
                if (half_n & 1) {
                        stats->lower_quartile = x[half_half_n];
                        stats->upper_quartile = x[half_n + half_half_n + 1];
                } else {
                        stats->lower_quartile = (x[half_half_n - 1] + x[half_half_n]) / 2.0;
                        stats->upper_quartile = (x[half_n + half_half_n] + x[half_n + half_half_n + 1]) / 2.0;
                }
        } else {
                stats->median = (x[half_n - 1] + x[half_n]) / 2.0;
                if (half_n & 1) {
                        stats->lower_quartile = x[half_half_n];
                        stats->upper_quartile = x[half_n + half_half_n];
                } else {
                        stats->lower_quartile = (x[half_half_n - 1] + x[half_half_n]) / 2.0;
                        stats->upper_quartile = (x[half_n + half_half_n - 1] + x[half_n + half_half_n]) / 2.0;
                }
        }
        stats->interquartile_range = stats->upper_quartile - stats->lower_quartile;

        stats->min = DBL_MAX;
        stats->max = -DBL_MAX;
        for (int i = 0; i < n; ++i) {
                stats->mean += x[i];

                if (x[i] < stats->min) {
                        stats->min = x[i];
                }

                if (x[i] > stats->max) {
                        stats->max = x[i];
                }
        }
        stats->mean /= n;

        for (int i = 0; i < n; ++i) {
                double d = x[i] - stats->mean;
                stats->stdev += d*d;
        }

        stats->stdev /= (n - 1);
        stats->stdev = sqrt(stats->stdev);
}

void nx_istatistics_1d(struct NXStatistics1D *stats, int n, int *x)
{
        NX_ASSERT_PTR(stats);
        NX_ASSERT((x && n > 0) || (n == 0));

        memset(stats, 0, sizeof(*stats));
        stats->n = n;

        if (n == 0) {
                return;
        } else if (n == 1) {
                stats->mean = x[0];
                stats->median = x[0];
                stats->lower_quartile = x[0];
                stats->upper_quartile = x[0];
                stats->interquartile_range = 0.0;
                stats->stdev = 0.0;
                stats->min = x[0];
                stats->max = x[0];
                return;
        }

        qsort((void *)x, n, sizeof(int), nx_compare_int_incr);
        int half_n = n / 2;
        int half_half_n = half_n / 2;
        if (n & 1) {
                stats->median = x[half_n];
                if (half_n & 1) {
                        stats->lower_quartile = x[half_half_n];
                        stats->upper_quartile = x[half_n + half_half_n + 1];
                } else {
                        stats->lower_quartile = (x[half_half_n - 1] + x[half_half_n]) / 2.0;
                        stats->upper_quartile = (x[half_n + half_half_n] + x[half_n + half_half_n + 1]) / 2.0;
                }
        } else {
                stats->median = (x[half_n - 1] + x[half_n]) / 2.0;
                if (half_n & 1) {
                        stats->lower_quartile = x[half_half_n];
                        stats->upper_quartile = x[half_n + half_half_n];
                } else {
                        stats->lower_quartile = (x[half_half_n - 1] + x[half_half_n]) / 2.0;
                        stats->upper_quartile = (x[half_n + half_half_n - 1] + x[half_n + half_half_n]) / 2.0;
                }
        }
        stats->interquartile_range = stats->upper_quartile - stats->lower_quartile;

        int imin = INT_MAX;
        int imax = INT_MIN;
        for (int i = 0; i < n; ++i) {
                stats->mean += x[i];

                if (x[i] < imin) {
                        imin = x[i];
                }

                if (x[i] > imax) {
                        imax = x[i];
                }
        }
        stats->mean /= n;
        stats->min = imin;
        stats->max = imax;

        for (int i = 0; i < n; ++i) {
                double d = x[i] - stats->mean;
                stats->stdev += d*d;
        }

        stats->stdev /= (n - 1);
        stats->stdev = sqrt(stats->stdev);
}
