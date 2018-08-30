/**
 * @file nx_uniform_sampler.c
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
