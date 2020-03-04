/**
 * @file nx_usac.c
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
#include "virg/nexus/nx_usac.h"

#include "float.h"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_vec.h"

double nx_usac(int m, double *best_model, int min_n_samples,
               struct NXUSACConfig *config, void *data)
{
        NX_ASSERT(m > 0);
        NX_ASSERT_PTR(best_model);
        NX_ASSERT(min_n_samples >= 1);
        NX_ASSERT_PTR(config);

        // Initialize the current model and its score
        double best_score = -DBL_MAX;
        nx_dvec_set_zero(m, best_model);

        // Allocate buffers sample ids and for models assuming only a single
        // model will be estimated from the minimal sample. The model estimation
        // function should reallocate if multiple models are estimated.
        int max_n_models = 1;
        double *models = NX_NEW_D(max_n_models * m);

        double *refined_model = NX_NEW_D(m);

        int *sample_ids = NX_NEW_I(min_n_samples);

        // Main loop
        while (!config->termination_fn(best_model, best_score, data)) {
                // Sample and check minimal set
                config->sampling_fn(sample_ids, data);
                if (!config->sample_check_fn(sample_ids, data))
                        continue;

                // Estimate models from the samples
                int n_models = config->model_fit_fn(&max_n_models, &models,
                                                    sample_ids, data);

                // Check and score models to look for a better one
                for (int i = 0; i < n_models; ++i) {
                        double *model_i = models + m * i;
                        if (!config->model_check_fn(model_i, sample_ids, data))
                                continue;

                        double score = config->model_score_fn(model_i,
                                                              sample_ids,
                                                              data);

                        // Update the best model if possible
                        if (score > best_score) {
                                if (!config->degeneracy_check_fn(model_i,
                                                                 sample_ids,
                                                                 data))
                                        continue;

                                // If possible refine the model and check if score improved
                                if (config->local_refinement_fn(refined_model,
                                                                model_i,
                                                                sample_ids,
                                                                data)) {

                                        double local_score;
                                        local_score = config->model_score_fn(refined_model,
                                                                             sample_ids,
                                                                             data);
                                        if (local_score > score) {
                                                nx_dvec_copy(m, best_model, refined_model);
                                                best_score = local_score;
                                        } else {
                                                nx_dvec_copy(m, best_model, model_i);
                                                best_score = score;
                                        }
                                } else { // No refinement, simply update the best model and score
                                                nx_dvec_copy(m, best_model, model_i);
                                                best_score = score;
                                }
                        }
                }
        }

        // Refine the best model using all available data
        config->global_refinement_fn(refined_model, best_model, data);
        double global_score = config->model_score_fn(refined_model,
                                                     sample_ids,
                                                     data);
        if (global_score > best_score) {
                nx_dvec_copy(m, best_model, refined_model);
                best_score = global_score;
        }

        nx_free(sample_ids);
        nx_free(refined_model);
        nx_free(models);

        return best_score;
}
