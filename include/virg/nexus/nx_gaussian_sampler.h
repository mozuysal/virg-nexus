/**
 * @file nx_gaussian_sampler.h
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
#ifndef VIRG_NEXUS_NX_GAUSSIAN_SAMPLER_H
#define VIRG_NEXUS_NX_GAUSSIAN_SAMPLER_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_uniform_sampler.h"

__NX_BEGIN_DECL

struct NXGaussianSampler;

struct NXGaussianSampler *nx_gaussian_sampler_instance();

void nx_gaussian_sampler_instance_free();

struct NXGaussianSampler *nx_gaussian_sampler_new(struct NXUniformSampler *usampler);

struct NXGaussianSampler *nx_gaussian_sampler_new_with_seed(uint32_t seed);

void nx_gaussian_sampler_free(struct NXGaussianSampler *sampler);

void nx_gaussian_sampler_init_time(struct NXGaussianSampler *sampler);

void nx_gaussian_sampler_init_seed(struct NXGaussianSampler *sampler, uint32_t seed);

void nx_gaussian_sampler_sample_s(struct NXGaussianSampler *sampler,
                                  int n, float *samples,
                                  float mean, float sigma);

void nx_gaussian_sampler_sample_d(struct NXGaussianSampler *sampler,
                                  int n, double *samples,
                                  double mean, double sigma);

__NX_END_DECL

#endif
