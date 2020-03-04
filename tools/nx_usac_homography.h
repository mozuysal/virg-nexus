/**
 * @file nx_usac_homography.h
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
#ifndef VIRG_NEXUS_NX_USAC_HOMOGRAPHY_H
#define VIRG_NEXUS_NX_USAC_HOMOGRAPHY_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_point_match_2d.h"

__NX_BEGIN_DECL

int nx_usac_estimate_homography(double *h, int n_corr,
                                struct NXPointMatch2D *corr_list,
                                double inlier_tolerance, int max_n_iter);

__NX_END_DECL

#endif
