/**
 * @file nx_uniform_sampler.c
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
#include "virg/nexus/nx_uniform_sampler.h"

#include <stdlib.h>
#include <time.h>

#include "virg/nexus/nx_alloc.h"
#include "SFMT/SFMT.h"

struct NXUniformSampler {
        sfmt_t state;
};

static struct NXUniformSampler *g_uniform_sampler = NULL;

struct NXUniformSampler *nx_uniform_sampler_instance()
{
        if (!g_uniform_sampler) {
                g_uniform_sampler = nx_uniform_sampler_new();
        }

        return g_uniform_sampler;
}

void nx_uniform_sampler_instance_free()
{
        nx_uniform_sampler_free(g_uniform_sampler);
        g_uniform_sampler = NULL;
}

struct NXUniformSampler *nx_uniform_sampler_alloc()
{
        struct NXUniformSampler *sampler = NX_NEW(1, struct NXUniformSampler);
        return sampler;
}

struct NXUniformSampler *nx_uniform_sampler_new()
{
        struct NXUniformSampler *sampler = nx_uniform_sampler_alloc();
        nx_uniform_sampler_init_time(sampler);

        return sampler;
}

struct NXUniformSampler *nx_uniform_sampler_new_with_seed(uint32_t seed)
{
        struct NXUniformSampler *sampler = nx_uniform_sampler_alloc();
        nx_uniform_sampler_init_seed(sampler, seed);

        return sampler;
}

void nx_uniform_sampler_free(struct NXUniformSampler *sampler)
{
        if (sampler) {
                nx_free(sampler);
        }
}

void nx_uniform_sampler_init_time(struct NXUniformSampler *sampler)
{
        sfmt_init_gen_rand(&sampler->state, time(NULL));
}

void nx_uniform_sampler_init_seed(struct NXUniformSampler *sampler, uint32_t seed)
{
        sfmt_init_gen_rand(&sampler->state, seed);
}

uint32_t nx_uniform_sampler_sample32(struct NXUniformSampler *sampler)
{
        return sfmt_genrand_uint32(&sampler->state);
}

uint64_t nx_uniform_sampler_sample64(struct NXUniformSampler *sampler)
{
        return sfmt_genrand_uint64(&sampler->state);
}

void nx_uniform_sampler_fill32(struct NXUniformSampler *sampler, int n, uint32_t* samples)
{
        sfmt_fill_array32(&sampler->state, samples, n);
}

void nx_uniform_sampler_fill64(struct NXUniformSampler *sampler, int n, uint64_t* samples)
{
        sfmt_fill_array64(&sampler->state, samples, n);
}

float nx_uniform_sampler_sample_s(struct NXUniformSampler *sampler)
{
        return sfmt_genrand_real2(&sampler->state);
}

double nx_uniform_sampler_sample_d(struct NXUniformSampler *sampler)
{
        return sfmt_genrand_res53_mix(&sampler->state);
}
