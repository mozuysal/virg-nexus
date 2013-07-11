/**
 * @file nx_filter.h
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
#ifndef VIRG_NEXUS_NX_FILTER_H
#define VIRG_NEXUS_NX_FILTER_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

enum NXBorderMode {
        NX_BORDER_ZERO = 0,
        NX_BORDER_REPEAT,
        NX_BORDER_MIRROR
}

double nx_kernel_loss_gaussian(int n_k, double sigma);

short nx_kernel_sym_gaussian_si(int n_k, short *kernel, float sigma);

void nx_kernel_sym_gaussian_s(int n_k, float *kernel, float sigma);

void nx_kernel_sym_gaussian_d(int n_k, double *kernel, double sigma);

void nx_convolve_sym_si_uc(int n, uchar *data, int n_k, const short *kernel, const short kernel_sum);

void nx_convolve_sym_s_uc(int n, uchar *data, int n_k, const float *kernel);

void nx_convolve_sym_downsample2_si_uc(int n, uchar *data, int n_k, const short *kernel, const short kernel_sum);

void nx_convolve_sym_downsample2_s_uc(int n, uchar *data, int n_k, const float *kernel);

void nx_filter_copy_to_buffer1_uc(int n, uchar *buffer, const uchar *data, int n_border, enum NXBorderMode mode);

void nx_filter_copy_to_buffer_uc(int n, uchar *buffer, const uchar *data, int stride, int n_border, enum NXBorderMode mode);

__NX_END_DECL

#endif
