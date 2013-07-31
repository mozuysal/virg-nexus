/**
 * @file nx_uniform_sampler.h
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
#ifndef VIRG_NEXUS_NX_UNIFORM_SAMPLER_H
#define VIRG_NEXUS_NX_UNIFORM_SAMPLER_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

struct NXUniformSampler;

struct NXUniformSampler *nx_uniform_sampler_instance();

struct NXUniformSampler *nx_uniform_sampler_new();

void nx_uniform_sampler_free(struct NXUniformSampler *sampler);

void nx_uniform_sampler_init_time(struct NXUniformSampler *sampler);

void nx_uniform_sampler_init_seed(struct NXUniformSampler *sampler, uint32_t seed);

uint32_t nx_uniform_sampler_sample32(struct NXUniformSampler *sampler);

uint64_t nx_uniform_sampler_sample64(struct NXUniformSampler *sampler);

void nx_uniform_sampler_fill32(struct NXUniformSampler *sampler, int n, uint32_t* samples);

void nx_uniform_sampler_fill64(struct NXUniformSampler *sampler, int n, uint64_t* samples);

float nx_uniform_sampler_sample_s(struct NXUniformSampler *sampler);

double nx_uniform_sampler_sample_d(struct NXUniformSampler *sampler);

#define NX_UNIFORM_SAMPLE32 (nx_uniform_sampler_sample32(nx_uniform_sampler_instance()))

#define NX_UNIFORM_SAMPLE64 (nx_uniform_sampler_sample64(nx_uniform_sampler_instance()))

#define NX_UNIFORM_SAMPLE_S (nx_uniform_sampler_sample_s(nx_uniform_sampler_instance()))

#define NX_UNIFORM_SAMPLE_D (nx_uniform_sampler_sample_d(nx_uniform_sampler_instance()))

__NX_END_DECL

#endif
