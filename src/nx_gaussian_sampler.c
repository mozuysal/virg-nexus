/**
 * @file nx_gaussian_sampler.c
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
#include "virg/nexus/nx_gaussian_sampler.h"

#include <stdlib.h>
#include <math.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"

static struct NXGaussianSampler *g_gaussian_sampler = NULL;

struct NXGaussianSampler
{
        struct NXUniformSampler *usampler;
};

struct NXGaussianSampler *nx_gaussian_sampler_instance()
{
        if (!g_gaussian_sampler) {
                g_gaussian_sampler = nx_gaussian_sampler_new(NULL);
        }

        return g_gaussian_sampler;
}

void nx_gaussian_sampler_instance_free()
{
        nx_gaussian_sampler_free(g_gaussian_sampler);
        g_gaussian_sampler = NULL;
}

struct NXGaussianSampler *nx_gaussian_sampler_new(struct NXUniformSampler *usampler)
{
        struct NXGaussianSampler *sampler = NX_NEW(1, struct NXGaussianSampler);

        if (usampler)
                sampler->usampler = usampler;
        else {
                sampler->usampler = nx_uniform_sampler_new();
        }

        return sampler;
}

struct NXGaussianSampler *nx_gaussian_sampler_new_with_seed(uint32_t seed)
{
        struct NXGaussianSampler *sampler = nx_gaussian_sampler_new(NULL);
        nx_uniform_sampler_init_seed(sampler->usampler, seed);
        return sampler;
}

void nx_gaussian_sampler_free(struct NXGaussianSampler *sampler)
{
        if (sampler) {
                nx_uniform_sampler_free(sampler->usampler);
                nx_free(sampler);
        }
}

void nx_gaussian_sampler_init_time(struct NXGaussianSampler *sampler)
{
        NX_ASSERT_PTR(sampler);
        nx_uniform_sampler_init_time(sampler->usampler);
}

void nx_gaussian_sampler_init_seed(struct NXGaussianSampler *sampler, uint32_t seed)
{
        NX_ASSERT_PTR(sampler);
        nx_uniform_sampler_init_seed(sampler->usampler, seed);
}


/* ------------------ Sampling based on Marsaglia Polar Method ----------------

   do
    U1 = uniform()            /\* U1=[0,1] *\/
    U2 = uniform()            /\* U2=[0,1] *\/
    V1 = 2 * U1 -1            /\* V1=[-1,1] *\/
    V2 = 2 * U2 - 1           /\* V2=[-1,1] *\/
    S  = V1 * V1 + V2 * V2
   while S >=1

   X  = sqrt(-2 * log(S) / S) * V1
   X' = mean + sqrt(variance) * X
*/

void nx_gaussian_sampler_sample_s(struct NXGaussianSampler *sampler,
                                  int n, float *samples,
                                  float mean, float sigma)
{
        float u1;
        float u2;
        float v1, v2, s;

        float* s_ptr = samples;
        int i;
        for(i = 0; i < n - 1; i += 2) {
                float common;

                do {
                        u1 = nx_uniform_sampler_sample_s(sampler->usampler);
                        u2 = nx_uniform_sampler_sample_s(sampler->usampler);

                        v1 = 2.0f * u1 - 1.0f;
                        v2 = 2.0f * u2 - 1.0f;

                        s = v1 * v1 + v2 * v2;
                } while(s >= 1.0f);

                common = sigma * sqrtf(-2.0f * logf(s) / s);
                *s_ptr = common * v1 + mean;
                s_ptr++;
                *s_ptr = common * v2 + mean;
                s_ptr++;
        }

        if(i < n) {
                do {
                        u1 = nx_uniform_sampler_sample_s(sampler->usampler);
                        u2 = nx_uniform_sampler_sample_s(sampler->usampler);

                        v1 = 2.0f * u1 - 1.0f;
                        v2 = 2.0f * u2 - 1.0f;

                        s = v1 * v1 + v2 * v2;
                } while(s >= 1.0f);

                *s_ptr = sigma * sqrtf(-2.0f * logf(s) / s) * v1 + mean;
        }
}

void nx_gaussian_sampler_sample_d(struct NXGaussianSampler *sampler,
                                  int n, double *samples,
                                  double mean, double sigma)
{
        double u1;
        double u2;
        double v1, v2, s;

        double* s_ptr = samples;
        int i;
        for(i = 0; i < n - 1; i += 2) {
                double common;

                do {
                        u1 = nx_uniform_sampler_sample_s(sampler->usampler);
                        u2 = nx_uniform_sampler_sample_s(sampler->usampler);

                        v1 = 2.0 * u1 - 1.0;
                        v2 = 2.0 * u2 - 1.0;

                        s = v1 * v1 + v2 * v2;
                } while(s >= 1.0);

                common = sigma * sqrt(-2.0 * log(s) / s);
                *s_ptr = common * v1 + mean;
                s_ptr++;
                *s_ptr = common * v2 + mean;
                s_ptr++;
        }

        if (i < n) {
                do {
                        u1 = nx_uniform_sampler_sample_s(sampler->usampler);
                        u2 = nx_uniform_sampler_sample_s(sampler->usampler);

                        v1 = 2.0 * u1 - 1.0;
                        v2 = 2.0 * u2 - 1.0;

                        s = v1 * v1 + v2 * v2;
                } while(s >= 1.0);

                *s_ptr = sigma * sqrt(-2.0 * log(s) / s) * v1 + mean;
        }
}
