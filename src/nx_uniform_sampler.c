/**
 * @file nx_uniform_sampler.c
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
#include "virg/nexus/nx_uniform_sampler.h"

#include <stdlib.h>
#include <time.h>

#include "virg/nexus/nx_alloc.h"

struct SFMTState;

extern struct SFMTState *sfmt_new();
extern void sfmt_free(struct SFMTState *state);
extern uint32_t sfmt_gen_rand32(struct SFMTState *state);
extern uint64_t sfmt_gen_rand64(struct SFMTState *state);
extern void sfmt_fill_array32(struct SFMTState *state, uint32_t *array, int size);
extern void sfmt_fill_array64(struct SFMTState *state, uint64_t *array, int size);
extern void sfmt_init_gen_rand(struct SFMTState *state, uint32_t seed);
extern void sfmt_init_by_array(struct SFMTState *state, uint32_t *init_key, int key_length);
extern const char *sfmt_get_idstring(void);
extern int sfmt_get_min_array_size32(void);
extern int sfmt_get_min_array_size64(void);

struct NXUniformSampler {
        struct SFMTState *state;
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
        sampler->state = sfmt_new();

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
                sfmt_free(sampler->state);
                nx_free(sampler);
        }
}

void nx_uniform_sampler_init_time(struct NXUniformSampler *sampler)
{
        sfmt_init_gen_rand(sampler->state, time(NULL));
}

void nx_uniform_sampler_init_seed(struct NXUniformSampler *sampler, uint32_t seed)
{
        sfmt_init_gen_rand(sampler->state, seed);
}

uint32_t nx_uniform_sampler_sample32(struct NXUniformSampler *sampler)
{
        return sfmt_gen_rand32(sampler->state);
}

uint64_t nx_uniform_sampler_sample64(struct NXUniformSampler *sampler)
{
        return sfmt_gen_rand64(sampler->state);
}

void nx_uniform_sampler_fill32(struct NXUniformSampler *sampler, int n, uint32_t* samples)
{
        sfmt_fill_array32(sampler->state, samples, n);
}

void nx_uniform_sampler_fill64(struct NXUniformSampler *sampler, int n, uint64_t* samples)
{
        sfmt_fill_array64(sampler->state, samples, n);
}

float nx_uniform_sampler_sample_s(struct NXUniformSampler *sampler)
{
        return sfmt_gen_rand32(sampler->state) * (1.0 / 4294967296.0);
}

double nx_uniform_sampler_sample_d(struct NXUniformSampler *sampler)
{
        return sfmt_gen_rand64(sampler->state) * (1.0 / 18446744073709551616.0L);
}
