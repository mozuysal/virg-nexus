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

/**
 * How to handle pixels at the edge of the data range.
 */
enum NXBorderMode {
        NX_BORDER_ZERO = 0,     /**< Border pixels are all zeros */
        NX_BORDER_REPEAT,       /**< The last pixel repeats along the border */
        NX_BORDER_MIRROR        /**< The pixels are mirrored around the last pixel */
};

/**
 * Calculates the lost area under the Gaussian curve by using a kernel of length n.
 *
 * @param n Total width of the kernel
 * @param sigma Gaussian standard deviation
 *
 * @return The percentage of the area that is not covered by the kernel in [0, 1]
 */
double nx_kernel_loss_gaussian(int n, double sigma);

/**
 * Computes the symmetric Gaussian kernel with integer precision. The last
 * element of the kernel is always equak to 1.
 *
 * @param n_k Number of elements necessary to store the symmetric kernel, must be > 1
 * @param kernel An array to store the center and right side elements of the kernel
 * @param sigma Gaussian standard deviation
 *
 * @return The sum of the elements of the kernel (including the symmetric part)
 */
short nx_kernel_sym_gaussian_si(int n_k, short *kernel, float sigma);

/**
 * @defgroup nx_kernel_sym_sd Functions related to computation of floating-point symmetric kernels
 * Computes the symmetric Gaussian kernel with single precision. The kernel
 * elements sum up to 1.0f.
 *
 * @param n_k Number of elements necessary to store the symmetric kernel, must be > 1
 * @param kernel An array to store the center and right side elements of the kernel
 * @param sigma Gaussian standard deviation
 * @{
 */
void nx_kernel_sym_gaussian_s(int n_k, float *kernel, float sigma);
void nx_kernel_sym_gaussian_d(int n_k, double *kernel, double sigma);
/**
 * @}
*/

/**
 * Convolves a unsigned char data buffer using a symmetric kernel of length
 * (2*n_k+1) in integer precision.
 *
 * @param n Number of elements to convolve not including the border values
 * @param data Pointer to the beginning of the buffer
 * @param n_k Number of elements in the kernel array
 * @param kernel The kernel
 * @param kernel_sum Sum of the elements of the kernel (including the symmetric part)
 */
void nx_convolve_sym_si_uc(int n, uchar *data, int n_k, const short *kernel, const short kernel_sum);

/**
 * Convolves unsigned char data buffer using a symmetric kernel of length
 * (2*n_k+1) in single precision.
 *
 * @param n Number of elements to convolve not including the border values
 * @param data Pointer to the beginning of the buffer
 * @param n_k Number of elements in the kernel array
 * @param kernel The kernel
 */
void nx_convolve_sym_s_uc(int n, uchar *data, int n_k, const float *kernel);

/**
 * Convolves a unsigned char data buffer using a symmetric kernel of length
 * (2*n_k+1) in integer precision and downsample by two.
 *
 * @param n Number of elements to convolve not including the border values
 * @param data Pointer to the beginning of the buffer
 * @param n_k Number of elements in the kernel array
 * @param kernel The kernel
 * @param kernel_sum Sum of the elements of the kernel
 */
void nx_convolve_sym_downsample2_si_uc(int n, uchar *data, int n_k, const short *kernel, const short kernel_sum);

/**
 * Convolves unsigned char data buffer using a symmetric kernel of length
 * (2*n_k+1) in single precision and downsample by two.
 *
 * @param n Number of elements to convolve not including the border values
 * @param data Pointer to the beginning of the buffer
 * @param n_k Number of elements in the kernel array
 * @param kernel The kernel
 */
void nx_convolve_sym_downsample2_s_uc(int n, uchar *data, int n_k, const float *kernel);

/**
 * Copies n consecutive unsigned char values from a data vector into a buffer
 * and creates a border around the data elements.
 *
 * @param n Number of elements to copy
 * @param buffer Target buffer pointer
 * @param data Source data pointer
 * @param n_border Number of border elements
 * @param mode Buffer fill mode
 */
void nx_filter_copy_to_buffer1_uc(int n, uchar *buffer, const uchar *data, int n_border, enum NXBorderMode mode);

/**
 * Copies n strided unsigned char values from a data vector into a buffer and creates a
 * border around the data elements.
 *
 * @param n Number of elements to copy
 * @param buffer Target buffer pointer
 * @param data Source data pointer
 * @param stride Stride between source data elements
 * @param n_border Number of border elements
 * @param mode Buffer fill mode
 */
void nx_filter_copy_to_buffer_uc(int n, uchar *buffer, const uchar *data, int stride, int n_border, enum NXBorderMode mode);

/**
 * Allocates an unsigned uchar buffer that will be used for convolution.
 *
 * @param n Number of data elements to be convolved
 * @param n_border Number of border elements at one side
 *
 * @return Allocated buffer pointer, must be freed by nx_free()
 */
uchar *nx_filter_buffer_alloc(int n, int n_border);

/**
 * Allocates an unsigned uchar buffer that will be used for convolution using
 * SIMD instructions.
 *
 * @param n Number of data elements to be convolved
 * @param n_border Number of border elements at one side
 *
 * @return Allocated buffer pointer aligned according to SIMD requirements, must
 * be freed by nx_free()
 */
uchar *nx_filter_simd_buffer_alloc(int n, int n_border);

__NX_END_DECL

#endif
