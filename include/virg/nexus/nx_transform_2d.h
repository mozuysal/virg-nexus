/**
 * @file nx_transform_2d.h
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
 * You should have received a copy of the GNU General Public License along with
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_TRANSFORM_2D_H
#define VIRG_NEXUS_NX_TRANSFORM_2D_H

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

void nx_transform_2d_fill_similarity_s(float *t, float dx_post, float dy_post, float lambda,
                                       float theta, float dx_pre, float dy_pre);
void nx_transform_2d_fill_similarity_d(double *t, double dx_post, double dy_post, double lambda,
                                       double theta, double dx_pre, double dy_pre);

void nx_transform_2d_fill_affine_s(float *t, float dx_post, float dy_post, float lambda,
                                   float psi, float theta, float phi,
                                   float dx_pre, float dy_pre);
void nx_transform_2d_fill_affine_d(double *t, double dx_post, double dy_post, double lambda,
                                   double psi, double theta, double phi,
                                   double dx_pre, double dy_pre);

float  nx_transform_2d_fill_perspective_s(float *t,  const float *corners);
double nx_transform_2d_fill_perspective_d(double *t, const double *corners);


__NX_END_DECL

#endif
