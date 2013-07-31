/**
 * @file nx_spline.c
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Izmir Institute of Technology. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 * Spline interpolation and derivative computation as explained in
 *
 * "Interpolation Revisited", P. Thévenaz, T. Blu, and M. Unser,
 * IEEE Trans. on Medical Imaging, Vol. 9, No. 7, 2000
 *
 * http://bigwww.epfl.ch/thevenaz/interpolation/
 */
#include "virg/nexus/nx_spline.h"

#include <math.h>
#include <float.h>

#include "virg/nexus/nx_assert.h"

void nx_spline_compute_1d(int n, float *coeff, int stride)
{
        NX_ASSERT_PTR(coeff);

        // Pole for cubic spline
        float pole      = sqrt(3.0) - 2.0;
        float ipole     = 1.0 / pole;
        float lambda    = (1.0 - pole) * (1.0 - 1.0 / pole);
        float tolerance = FLT_EPSILON;

        float antiCausalScaleFactor = pole / (pole * pole - 1.0);

        if (n == 1) {
                return;
        }

        static int lastN = 1;
        static float z2nBuffer;

        float* coeff_ptr = coeff;
        for(int i = 0; i < n; ++i, coeff_ptr += stride) {
                *coeff_ptr *= lambda;
        }

        // Casual initialization
        float zn = pole;
        float sum = 0.0;

        int horizon = (int)ceil(log(tolerance) / log(fabs(pole)));
        coeff_ptr = coeff;
        if (horizon < n) {
                sum = *coeff_ptr;

                coeff_ptr += stride;
                for (int i = 1; i < horizon; ++i, coeff_ptr += stride) {
                        sum += zn * *coeff_ptr;
                        zn *= pole;
                }
        } else {
                if (n != lastN) {
                        lastN = n;
                        z2nBuffer  = pow((double)pole, (double)(n - 1));
                }

                float z2n = z2nBuffer;

                sum = *coeff_ptr + z2n * *(coeff_ptr + (n - 1) * stride);
                z2n  *= z2n * ipole;

                coeff_ptr += stride;
                for (int i = 1; i < n - 2; ++i, coeff_ptr += stride) {
                        sum += (zn + z2n) * *coeff_ptr;
                        zn  *= pole;
                        z2n *= ipole;
                }

                sum /= (1.0 - zn * zn);
        }

        coeff[0] = sum;

        // Causal loop
        coeff_ptr = coeff + stride;
        for (int i = 1; i < n; ++i, coeff_ptr += stride) {
                *coeff_ptr += pole * *(coeff_ptr - stride);
        }

        // Anticausal initialization
        coeff_ptr = coeff + (n - 1) * stride;
        *coeff_ptr = antiCausalScaleFactor * (pole * *(coeff_ptr - stride) + *coeff_ptr);

        // Anticausal loop
        coeff_ptr = coeff + (n - 2) * stride;
        for (int i = n - 2; 0 <= i; --i, coeff_ptr -= stride) {
                *coeff_ptr = pole * (*(coeff_ptr + stride) - *coeff_ptr);
        }
}

void nx_spline_derivative_1d(int n, float *dx, const float *coeff)
{
        NX_ASSERT_PTR(dx);
        NX_ASSERT_PTR(coeff);

        dx[0] = 0.0f;
        dx[n-1] = 0.0f;

        for (int i = n-2; i > 0; --i) {
                dx[i] = 0.5f * (coeff[i+1] - coeff[i-1]);
        }

}

void nx_spline_derivative_1d_strided(int n, float *dx, int dx_stride, const float* coeff, int coeff_stride)
{
        NX_ASSERT_PTR(dx);
        NX_ASSERT_PTR(coeff);

        dx[0] = 0.0f;
        dx[(n-1)*dx_stride] = 0.0f;

        const float* cp_ptr  = coeff + (n-1)*coeff_stride;
        const float* cm_ptr  = coeff + (n-3)*coeff_stride;
        float* dx_ptr = dx + (n-2) * dx_stride;
        for (int i = n-2; i > 0; --i, dx_ptr -= dx_stride, cp_ptr -= coeff_stride, cm_ptr -= coeff_stride) {
                *dx_ptr = 0.5 * (*cp_ptr - *cm_ptr);
        }
}

float nx_spline_interp_2d(float x, float y, int width, int height, const float* coeff, int stride)
{
        NX_ASSERT_PTR(coeff);

        int i = (int)floor(x) - 1;
        int j = (int)floor(y) - 1;

        float oneOverSix = 1.0f / 6.0f;

        float wx   = x - (float)(i + 1);
        float xW3  = oneOverSix * wx * wx * wx;
        float tmpX = oneOverSix + 0.5f * wx * (wx - 1.0f);
        float xW0  = tmpX - xW3;
        float xW2  = wx + xW0 - 2.0f * xW3;
        float xW1  = 1.0f - xW2 - tmpX;

        float wy   = y - (float)(j + 1);
        float yW3  = oneOverSix * wy * wy * wy;
        float tmpY = oneOverSix + 0.5f * wy * (wy - 1.0f);
        float yW0  = tmpY - yW3;
        float yW2  = wy + yW0 - 2.0f * yW3;
        float yW1  = 1.0f - yW2 - tmpY;

        int yId[4] = {j, j + 1, j + 2, j + 3};
        if (j < 0) {
                int h2 = 2 * height - 2;
                if (yId[0] < 0) yId[0] =  -yId[0] - h2 * (-yId[0] / h2);
                if (yId[1] < 0) yId[1] =  -yId[1] - h2 * (-yId[1] / h2);
                if (yId[2] < 0) yId[2] =  -yId[2] - h2 * (-yId[2] / h2);
                if (yId[3] < 0) yId[3] =  -yId[3] - h2 * (-yId[3] / h2);
        }
        else if (j >= height - 3 ) {
                int h2 = 2 * height - 2;
                if(yId[0] >= height) yId[0] = h2 - yId[0] - h2 * (yId[0] / h2);
                if(yId[1] >= height) yId[1] = h2 - yId[1] - h2 * (yId[1] / h2);
                if(yId[2] >= height) yId[2] = h2 - yId[2] - h2 * (yId[2] / h2);
                if(yId[3] >= height) yId[3] = h2 - yId[3] - h2 * (yId[3] / h2);
        }

        const float* rowH[4] = { coeff + stride*yId[0], coeff + stride*yId[1],
                                 coeff + stride*yId[2], coeff + stride*yId[3] };

        int xId[4] = {i, i+1, i+2, i+3};
        if (i < 0) {
                int w2 = 2*width - 2;
                if(xId[0] < 0) xId[0] =  -xId[0] - w2 * (-xId[0] / w2);
                if(xId[1] < 0) xId[1] =  -xId[1] - w2 * (-xId[1] / w2);
                if(xId[2] < 0) xId[2] =  -xId[2] - w2 * (-xId[2] / w2);
                if(xId[3] < 0) xId[3] =  -xId[3] - w2 * (-xId[3] / w2);
        }
        else if(i >= width - 3) {
                int w2 = 2*width - 2;
                if(xId[0] >= width) xId[0] = w2 - xId[0] - w2 * (xId[0] / w2);
                if(xId[1] >= width) xId[1] = w2 - xId[1] - w2 * (xId[1] / w2);
                if(xId[2] >= width) xId[2] = w2 - xId[2] - w2 * (xId[2] / w2);
                if(xId[3] >= width) xId[3] = w2 - xId[3] - w2 * (xId[3] / w2);
        }

        float w0 = xW0 * rowH[0][xId[0]] + xW1 * rowH[0][xId[1]] + xW2 * rowH[0][xId[2]] + xW3 * rowH[0][xId[3]];
        float w1 = xW0 * rowH[1][xId[0]] + xW1 * rowH[1][xId[1]] + xW2 * rowH[1][xId[2]] + xW3 * rowH[1][xId[3]];
        float w2 = xW0 * rowH[2][xId[0]] + xW1 * rowH[2][xId[1]] + xW2 * rowH[2][xId[2]] + xW3 * rowH[2][xId[3]];
        float w3 = xW0 * rowH[3][xId[0]] + xW1 * rowH[3][xId[1]] + xW2 * rowH[3][xId[2]] + xW3 * rowH[3][xId[3]];

        return yW0*w0 + yW1*w1 + yW2*w2 + yW3*w3;
}

void nx_spline_from_grayscale(int width, int height, float *coeff, int coeff_stride, const uchar *img, int img_stride)
{
        NX_ASSERT_PTR(coeff);
        NX_ASSERT_PTR(img);
        NX_ASSERT(width > 0);
        NX_ASSERT(height > 0);
        NX_ASSERT(coeff_stride >= width);
        NX_ASSERT(img_stride >= width);

        for (int y = 0; y < height; ++y) {
                const uchar* row_img = img + y*img_stride;
                float* row_coeff = coeff + y*coeff_stride;
                for (int x = 0; x < width; ++x) {
                        row_coeff[x] = (float)row_img[x];
                }
        }

        for (int y = 0; y < height; ++y) {
                nx_spline_compute_1d(width, coeff + y*coeff_stride, 1);
        }

        for (int x = 0; x < width; ++x) {
                nx_spline_compute_1d(height, coeff + x, coeff_stride);
        }
}
