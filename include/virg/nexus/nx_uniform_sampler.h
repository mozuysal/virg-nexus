/**
 * @file nx_uniform_sampler.h
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
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
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

void nx_uniform_sampler_instance_free();

struct NXUniformSampler *nx_uniform_sampler_alloc();

struct NXUniformSampler *nx_uniform_sampler_new();

struct NXUniformSampler *nx_uniform_sampler_new_with_seed(uint32_t seed);

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
