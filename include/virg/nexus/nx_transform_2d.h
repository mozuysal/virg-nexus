/**
 * @file nx_transform_2d.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Izmir Institute of Technology. All rights reserved.
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
