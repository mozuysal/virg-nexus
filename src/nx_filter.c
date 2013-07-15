/**
 * @file nx_filter.c
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
#include "virg/nexus/nx_filter.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_math.h"

static void fill_buffer_border_uc(int n, uchar *buffer, int n_border, enum NXBorderMode mode);

/**
 * Calculates the lost area under the Gaussian curve by using a kernel of length n.
 *
 * @param n Total width of the kernel
 * @param sigma Gaussian standard deviation
 *
 * @return The percentage of the area that is not covered by the kernel in [0, 1]
 */
double nx_kernel_loss_gaussian(int n, double sigma)
{
        NX_ASSERT(n > 0);
        NX_ASSERT(sigma > 0);

        double erf_f = 1.0 / (sqrt(2.0)*sigma);
        double g_n_plus = 0.5 * nx_erf((n+1) * 0.5 * erf_f);
        double g_n = 2.0 * g_n_plus;

        return 1.0 - g_n;
}

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
short nx_kernel_sym_gaussian_si(int n_k, short *kernel, float sigma)
{
        NX_ASSERT(n_k > 1);
        NX_ASSERT_PTR(kernel);
        NX_ASSERT(sigma > 0);

        double erf_f = 1.0 / (sqrt(2.0)*sigma);
        double g_n_plus  = 0.5 * nx_erf((n_k - 0.5) * erf_f);
        double g_n_minus = 0.5 * nx_erf((n_k - 1.5) * erf_f);
        double g_n_inv = 1.0 / (g_n_plus - g_n_minus);

        kernel[n_k-1] = 1;
        short sum = 2 * kernel[n_k-1];
        double g_i_plus = g_n_minus;
        double g_i_minus;
        for (int i = n_k-2; i > 0; --i) {
                g_i_minus = 0.5 * nx_erf((i - 0.5) * erf_f);
                kernel[i] = g_n_inv * (g_i_plus - g_i_minus);
                sum += 2 * kernel[i];
                g_i_plus = g_i_minus;
        }

        // g_i_minus = g_0.5
        kernel[0] = g_n_inv * 2.0 * g_i_minus;
        sum += kernel[0];

        return sum;
}

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
void nx_kernel_sym_gaussian_s(int n_k, float *kernel, float sigma)
{
        NX_ASSERT(n_k > 1);
        NX_ASSERT_PTR(kernel);
        NX_ASSERT(sigma > 0);

        double erf_f = 1.0 / (sqrt(2.0)*sigma);

        kernel[0] = nx_erf(0.5 * erf_f);
        float sum = kernel[0];
        double g_i_minus = 0.5 * kernel[0];
        for (int i = 1; i < n_k; ++i) {
                double g_i_plus = 0.5 * nx_erf((i + 0.5) * erf_f);
                kernel[i] = g_i_plus - g_i_minus;
                sum += 2.0f * kernel[i];
                g_i_minus = g_i_plus;
        }

        float sum_inv = 1.0f / sum;
        for (int i = 0; i < n_k; ++i) {
                kernel[i] *= sum_inv;
        }
}

void nx_kernel_sym_gaussian_d(int n_k, double *kernel, double sigma)
{
        NX_ASSERT(n_k > 1);
        NX_ASSERT_PTR(kernel);
        NX_ASSERT(sigma > 0);

        double erf_f = 1.0 / (sqrt(2.0)*sigma);

        kernel[0] = nx_erf(0.5 * erf_f);
        double sum = kernel[0];
        double g_i_minus = 0.5 * kernel[0];
        for (int i = 1; i < n_k; ++i) {
                double g_i_plus = 0.5 * nx_erf((i + 0.5) * erf_f);
                kernel[i] = g_i_plus - g_i_minus;
                sum += 2.0 * kernel[i];
                g_i_minus = g_i_plus;
        }

        double sum_inv = 1.0 / sum;
        for (int i = 0; i < n_k; ++i) {
                kernel[i] *= sum_inv;
        }
}
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
void nx_convolve_sym_si_uc(int n, uchar *data, int n_k, const short *kernel, const short kernel_sum)
{
        NX_ASSERT(n > 1);
        NX_ASSERT_PTR(data);
        NX_ASSERT(n_k > 1);
        NX_ASSERT_PTR(kernel);
        NX_ASSERT(kernel_sum > 0);

        float norm_factor = 1.0f / kernel_sum;

        for (int i = 0; i < n; ++i) {
                uchar* dk0 = data + i + n_k - 1;
                int sum = kernel[0] * *dk0;
                for (int k = 1; k < n_k; ++k) {
                        sum += kernel[k] * (dk0[-k] + dk0[+k]);
                }
                data[i] = norm_factor * sum;
        }
}

/**
 * Convolves unsigned char data buffer using a symmetric kernel of length
 * (2*n_k+1) in single precision.
 *
 * @param n Number of elements to convolve not including the border values
 * @param data Pointer to the beginning of the buffer
 * @param n_k Number of elements in the kernel array
 * @param kernel The kernel
 */
void nx_convolve_sym_s_uc(int n, uchar *data, int n_k, const float *kernel)
{
        NX_ASSERT(n > 1);
        NX_ASSERT_PTR(data);
        NX_ASSERT(n_k > 1);
        NX_ASSERT_PTR(kernel);

        for (int i = 0; i < n; ++i) {
                uchar* dk0 = data + i + n_k - 1;
                float sum = kernel[0] * *dk0;
                for (int k = 1; k < n_k; ++k) {
                        sum += kernel[k] * (dk0[-k] + dk0[+k]);
                }
                data[i] = sum;
        }
}

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
void nx_convolve_sym_downsample2_si_uc(int n, uchar *data, int n_k, const short *kernel, const short kernel_sum)
{
        NX_ASSERT(n > 1);
        NX_ASSERT_PTR(data);
        NX_ASSERT(n_k > 1);
        NX_ASSERT_PTR(kernel);
        NX_ASSERT(kernel_sum > 0);

        float norm_factor = 1.0f / kernel_sum;

        for (int i = 0; i < n; i += 2) {
                uchar* dk0 = data + i + n_k - 1;
                int sum = kernel[0] * *dk0;
                for (int k = 1; k < n_k; ++k) {
                        sum += kernel[k] * (dk0[-k] + dk0[+k]);
                }

                data[i/2] = norm_factor * sum;
        }
}

/**
 * Convolves unsigned char data buffer using a symmetric kernel of length
 * (2*n_k+1) in single precision and downsample by two.
 *
 * @param n Number of elements to convolve not including the border values
 * @param data Pointer to the beginning of the buffer
 * @param n_k Number of elements in the kernel array
 * @param kernel The kernel
 */
void nx_convolve_sym_downsample2_s_uc(int n, uchar *data, int n_k, const float *kernel)
{
        NX_ASSERT(n > 1);
        NX_ASSERT_PTR(data);
        NX_ASSERT(n_k > 1);
        NX_ASSERT_PTR(kernel);

        for (int i = 0; i < n; i += 2) {
                uchar* dk0 = data + i + n_k - 1;
                float sum = kernel[0] * *dk0;
                for (int k = 1; k < n_k; ++k) {
                        sum += kernel[k] * (dk0[-k] + dk0[+k]);
                }
                data[i/2] = sum;
        }
}

void fill_buffer_border_uc(int n, uchar *buffer, int n_border, enum NXBorderMode mode)
{
        uchar buffer_b;
        switch (mode) {
        case NX_BORDER_ZERO:
                memset(buffer, 0, n_border * sizeof(uchar));
                memset(buffer+n+n_border, 0, n_border * sizeof(uchar));
                break;
        case NX_BORDER_REPEAT:
                buffer_b = buffer[n_border];
                for (int i = 0; i < n_border; ++i)
                        buffer[i] = buffer_b;
                buffer_b = buffer[n_border+n-1];
                for (int i = 0; i < n_border; ++i)
                        buffer[n_border+n+i] = buffer_b;
                break;
        case NX_BORDER_MIRROR:
                for (int i = 0; i < n_border; ++i)
                        buffer[i] = buffer[2*n_border-i];
                for (int i = 0; i < n_border; ++i)
                        buffer[n_border+n+i] = buffer[n_border+n-i-2];
                break;
        }
}

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
void nx_filter_copy_to_buffer1_uc(int n, uchar *buffer, const uchar *data, int n_border, enum NXBorderMode mode)
{
        NX_ASSERT(n > 1);
        NX_ASSERT_PTR(buffer);
        NX_ASSERT_PTR(data);

        memcpy(buffer + n_border, data, n*sizeof(uchar));
        fill_buffer_border_uc(n, buffer, n_border, mode);
}

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
void nx_filter_copy_to_buffer_uc(int n, uchar *buffer, const uchar *data, int stride, int n_border, enum NXBorderMode mode)
{
        NX_ASSERT(n > 1);
        NX_ASSERT_PTR(buffer);
        NX_ASSERT_PTR(data);

        for (int i = 0; i < n; ++i) {
                buffer[n_border + i] = data[i * stride];
        }
        fill_buffer_border_uc(n, buffer, n_border, mode);
}

/**
 * Allocates an unsigned uchar buffer that will be used for convolution.
 *
 * @param n Number of data elements to be convolved
 * @param n_border Number of border elements at one side
 *
 * @return Allocated buffer pointer, must be freed by nx_free()
 */
uchar *nx_filter_buffer_alloc(int n, int n_border)
{
#if defined(VIRG_NEXUS_USE_SIMD)
        return nx_filter_simd_buffer_alloc(n, n_border);
#else
        size_t l = n + 2 * n_border;
        return nx_new_uc(l);
#endif
}

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
uchar *nx_filter_simd_buffer_alloc(int n, int n_border)
{
        size_t l = n + 2 * n_border;
        return (uchar *)nx_aligned_alloc(l * sizeof(uchar), NX_SIMD_ALIGNMENT);
}
