/**
 * @file nx_point_match_2d_stats.h
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
#ifndef VIRG_NEXUS_NX_POINT_MATCH_2D_STATS_H
#define VIRG_NEXUS_NX_POINT_MATCH_2D_STATS_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_point_match_2d.h"

__NX_BEGIN_DECL

struct NXPointMatch2DStats {
        double m[2]; // mean point
        double mp[2]; // mean point

        double d; // distance to mean
        double dp; // distance to mean
};

struct NXPointMatch2DStats *nx_point_match_2d_stats_new();

void nx_point_match_2d_stats_free(struct NXPointMatch2DStats *stats);

void nx_point_match_2d_stats_reset(struct NXPointMatch2DStats *stats);

void nx_point_match_2d_stats_normalize_matches(struct NXPointMatch2DStats *stats,
                                               int n_corr,
                                               struct NXPointMatch2D *corr_list);

void nx_point_match_2d_stats_denormalize_matches(const struct NXPointMatch2DStats *stats,
                                                 int n_corr,
                                                 struct NXPointMatch2D *corr_list);

void nx_point_match_2d_stats_denormalize_homography(const struct NXPointMatch2DStats *stats,
                                                    double *h);
void nx_point_match_2d_stats_denormalize_fundamental(const struct NXPointMatch2DStats *stats,
                                                     double *f);


__NX_END_DECL

#endif
