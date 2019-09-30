/**
 * @file nx_filter.h
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
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_FILTER_H
#define VIRG_NEXUS_NX_FILTER_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

/**
 * How to handle pixels at the edge of the data range.
 */
enum NXBorderMode {
        NX_BORDER_ZERO = 0,     /**< Border pixels are all zeros */
        NX_BORDER_REPEAT,       /**< The last pixel repeats along the border */
        NX_BORDER_MIRROR        /**< The pixels are mirrored around the last pixel */
};

double nx_kernel_loss_gaussian(int n, double sigma);

int nx_kernel_size_min_gaussian(double sigma, double loss_threshold);

void nx_kernel_sym_gaussian(int n_k, float *kernel, float sigma);

void nx_convolve_sym(int n, float *data, int n_k, const float *kernel);

void nx_convolve_box(int n, float *data, int n_r);

void nx_filter_copy_to_buffer1_uc(int n, float *buffer, const uchar *data, int n_border, enum NXBorderMode mode);

void nx_filter_copy_to_buffer_uc(int n, float *buffer, const uchar *data, int stride, int n_border, enum NXBorderMode mode);

void nx_filter_copy_to_buffer1(int n, float *buffer, const float *data, int n_border, enum NXBorderMode mode);

void nx_filter_copy_to_buffer(int n, float *buffer, const float *data, int stride, int n_border, enum NXBorderMode mode);

float *nx_filter_buffer_alloc(int n, int n_border);

__NX_END_DECL

#endif
