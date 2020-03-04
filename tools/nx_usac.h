/**
 * @file nx_usac.h
 *
 * Copyright (C) 2020 Mustafa Ozuysal. All rights reserved.
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
#ifndef VIRG_NEXUS_NX_USAC_H
#define VIRG_NEXUS_NX_USAC_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

typedef void (*NXUSACSamplingFn)(int *sample_ids,
                                 void *data);
typedef NXBool (*NXUSACSSampleCheckFn)(const int *sample_ids,
                                       void *data);
typedef int (*NXUSACModelFitFn)(int *n_max_models, double **models,
                                const int *sample_ids,
                                void *data);
typedef NXBool (*NXUSACModelCheckFn)(double *model, const int *sample_ids,
                                     void *data);
typedef double (*NXUSACModelScoreFn)(double *model, const int *sample_ids,
                                     void *data);
typedef NXBool (*NXUSACDegeneracyCheckFn)(double *model, const int *sample_ids,
                                          void *data);
typedef NXBool (*NXUSACLocalRefinementFn)(double *new_model, const double *model,
                                          const int *sample_ids, void *data);
typedef void (*NXUSACGlobalRefinementFn)(double *new_model, const double *model,
                                         void *data);
typedef NXBool (*NXUSACTerminationFn)(const double *best_model,
                                      double best_score, void *data);

struct NXUSACConfig {
        NXUSACSamplingFn sampling_fn;
        NXUSACSSampleCheckFn sample_check_fn;
        NXUSACModelFitFn model_fit_fn;
        NXUSACModelCheckFn model_check_fn;
        NXUSACModelScoreFn model_score_fn;
        NXUSACDegeneracyCheckFn degeneracy_check_fn;
        NXUSACLocalRefinementFn local_refinement_fn;
        NXUSACGlobalRefinementFn global_refinement_fn;
        NXUSACTerminationFn termination_fn;
};

double nx_usac(int m, double *model, int min_n_samples,
               struct NXUSACConfig *config, void *data);

__NX_END_DECL

#endif
