/**
 * @file nx_spline.h
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
#ifndef VIRG_NEXUS_NX_SPLINE_H
#define VIRG_NEXUS_NX_SPLINE_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

void nx_spline_compute_1d(int n, float *coeff, int stride);

void nx_spline_derivative_1d(int n, float *dx, const float *coeff);

void nx_spline_derivative_1d_strided(int n, float *dx, int dx_stride, const float* coeff, int coeff_stride);

float nx_spline_interp_2d(float x, float y, int width, int height, const float* coeff, int stride);

void nx_spline_from_grayscale(int width, int height, float *coeff, int coeff_stride, const uchar *img, int img_stride);

__NX_END_DECL

#endif
