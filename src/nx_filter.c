/**
 * @file nx_filter.c
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Mustafa Ozuysal. All rights reserved.
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

static inline void fill_buffer_border(int n, float *buffer, int n_border, enum NXBorderMode mode);
static inline double kernel_value_sym_gaussian(int i, double sigma);

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
        double g_n_plus = 0.5 * nx_erf(n * 0.5 * erf_f);
        double g_n = 2.0 * g_n_plus;

        return 1.0 - g_n;
}

/**
 * Calculates the minimum kernel size such that the loass is below the given
 * threshold.
 *
 * @param sigma Gaussian standard deviation
 * @param loss_threshold Maximum allowable loss (percentage of area under the Gaussian curve)
 *
 * @return Minimum kernel length
 */
int nx_kernel_size_min_gaussian(double sigma, double loss_threshold)
{
        int n = 3;
        while (nx_kernel_loss_gaussian(n, sigma) > loss_threshold) {
                n += 2;
        }

        return n;
}

double kernel_value_sym_gaussian(int i, double sigma)
{
        return exp(-0.5*i*i/(sigma*sigma));
}


/**
 * Computes the symmetric Gaussian kernel with single precision. The kernel
 * elements sum up to 1.0f.
 *
 * @param n_k Number of elements necessary to store the symmetric kernel, must be > 1
 * @param kernel An array to store the center and right side elements of the kernel
 * @param sigma Gaussian standard deviation
 * @{
 */
void nx_kernel_sym_gaussian(int n_k, float *kernel, float sigma)
{
        NX_ASSERT(n_k > 1);
        NX_ASSERT_PTR(kernel);
        NX_ASSERT(sigma > 0);

        kernel[0] = kernel_value_sym_gaussian(0, sigma);
        float sum = kernel[0];
        for (int i = 1; i < n_k; ++i) {
                kernel[i] = kernel_value_sym_gaussian(i, sigma);
                sum += 2.0f * kernel[i];
        }

        for (int i = 0; i < n_k; ++i) {
                kernel[i] /= sum;
        }
}

/**
 * Convolves float data buffer using a symmetric kernel of length
 * (2*n_k+1) in single precision.
 *
 * @param n Number of elements to convolve not including the border values
 * @param data Pointer to the beginning of the buffer
 * @param n_k Number of elements in the kernel array
 * @param kernel The kernel
 */
void nx_convolve_sym(int n, float *data, int n_k, const float *kernel)
{
        NX_ASSERT(n > 1);
        NX_ASSERT_PTR(data);
        NX_ASSERT(n_k > 1);
        NX_ASSERT_PTR(kernel);

        for (int i = 0; i < n; ++i) {
                float* dk0 = data + i + n_k - 1;
                float sum = kernel[0] * *dk0;
                for (int k = 1; k < n_k; ++k) {
                        sum += kernel[k] * (dk0[-k] + dk0[+k]);
                }
                data[i] = sum;
        }
}

void nx_convolve_box(int n, float *data, int n_r)
{
        NX_ASSERT(n > 1);
        NX_ASSERT_PTR(data);
        NX_ASSERT(n_r >= 1);

        float prev = data[0];
        for (int i = -n_r+1; i <= n_r; ++i)
                data[0] += data[i + n_r];
        for (int i = 1; i < n; ++i) {
                float di = data[i];
                data[i] = data[i-1] - prev + data[i+2*n_r];
                prev = di;
        }
        for (int i = 0; i < n; ++i)
                data[i] /= (2*n_r+1);
}

void fill_buffer_border(int n, float *buffer, int n_border, enum NXBorderMode mode)
{
        float buffer_b;
        switch (mode) {
        case NX_BORDER_ZERO:
                memset(buffer, 0, n_border * sizeof(float));
                memset(buffer+n+n_border, 0, n_border * sizeof(float));
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
        default:
                NX_FATAL(NX_LOG_TAG, "Unhandled switch case for border mode.");
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
void nx_filter_copy_to_buffer1_uc(int n, float *buffer, const uchar *data, int n_border, enum NXBorderMode mode)
{
        NX_ASSERT(n > 1);
        NX_ASSERT(n > n_border);
        NX_ASSERT_PTR(buffer);
        NX_ASSERT_PTR(data);

        for (int i = 0; i < n; ++i)
                buffer[n_border + i] = data[i];
        fill_buffer_border(n, buffer, n_border, mode);
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
void nx_filter_copy_to_buffer_uc(int n, float *buffer, const uchar *data, int stride, int n_border, enum NXBorderMode mode)
{
        NX_ASSERT(n > 1);
        NX_ASSERT(n > n_border);
        NX_ASSERT_PTR(buffer);
        NX_ASSERT_PTR(data);

        for (int i = 0; i < n; ++i)
                buffer[n_border + i] = data[i * stride];
        fill_buffer_border(n, buffer, n_border, mode);
}

/**
 * Copies n consecutive float values from a data vector into a buffer
 * and creates a border around the data elements.
 *
 * @param n Number of elements to copy
 * @param buffer Target buffer pointer
 * @param data Source data pointer
 * @param n_border Number of border elements
 * @param mode Buffer fill mode
 */
void nx_filter_copy_to_buffer1(int n, float *buffer, const float *data, int n_border, enum NXBorderMode mode)
{
        NX_ASSERT(n > 1);
        NX_ASSERT(n > n_border);
        NX_ASSERT_PTR(buffer);
        NX_ASSERT_PTR(data);

        memcpy(buffer + n_border, data, n*sizeof(float));
        fill_buffer_border(n, buffer, n_border, mode);
}

/**
 * Copies n strided float values from a data vector into a buffer and creates a
 * border around the data elements.
 *
 * @param n Number of elements to copy
 * @param buffer Target buffer pointer
 * @param data Source data pointer
 * @param stride Stride between source data elements
 * @param n_border Number of border elements
 * @param mode Buffer fill mode
 */
void nx_filter_copy_to_buffer(int n, float *buffer, const float *data, int stride, int n_border, enum NXBorderMode mode)
{
        NX_ASSERT(n > 1);
        NX_ASSERT(n > n_border);
        NX_ASSERT_PTR(buffer);
        NX_ASSERT_PTR(data);

        for (int i = 0; i < n; ++i)
                buffer[n_border + i] = data[i * stride];
        fill_buffer_border(n, buffer, n_border, mode);
}

/**
 * Allocates a float buffer that will be used for convolution.
 *
 * @param n Number of data elements to be convolved
 * @param n_border Number of border elements at one side
 *
 * @return Allocated buffer pointer, must be freed by nx_free()
 */
float *nx_filter_buffer_alloc(int n, int n_border)
{
        size_t l = n + 2 * n_border;

#if defined(VIRG_NEXUS_USE_SIMD)
        return (float *)nx_xaligned_alloc(l * sizeof(float), NX_SIMD_ALIGNMENT);
#else
        return NX_NEW_S(l);
#endif
}

