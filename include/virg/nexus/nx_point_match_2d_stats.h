/**
 * @file nx_point_match_2d_stats.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2019 Mustafa Ozuysal. All rights reserved.
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
