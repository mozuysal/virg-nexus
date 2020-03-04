/**
 * @file nx_usac_homography.c
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
#include "virg/nexus/nx_usac_homography.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "virg/nexus/nx_math.h"
#include "virg/nexus/nx_vec.h"
#include "virg/nexus/nx_uniform_sampler.h"
#include "virg/nexus/nx_homography.h"
#include "virg/nexus/nx_usac.h"

struct NXUSACHomographyData {
        int n_corr;
        struct NXPointMatch2D *corr_list;
        double inlier_tolerance;
        int max_n_iter;

        int n_top_hypo;
        int n_iterations;
};

void nx_usac_homography_sampling(int *sample_ids, void *data)
{
        struct NXUSACHomographyData *hdata = (struct NXUSACHomographyData *)data;

        sample_ids[0] = (int)(NX_UNIFORM_SAMPLE_S * hdata->n_top_hypo);

        do {
            sample_ids[1] = (int)(NX_UNIFORM_SAMPLE_S * hdata->n_top_hypo);
        } while(sample_ids[1]==sample_ids[0]);

        do {
            sample_ids[2] = (int)(NX_UNIFORM_SAMPLE_S * hdata->n_top_hypo);
        } while(sample_ids[2]==sample_ids[1] || sample_ids[2]==sample_ids[0]);

        do {
            sample_ids[3] = (int)(NX_UNIFORM_SAMPLE_S * hdata->n_top_hypo);
        } while(sample_ids[3]==sample_ids[1] || sample_ids[3]==sample_ids[1] || sample_ids[3]==sample_ids[0]);
}

NXBool nx_usac_check_homography_sample(const int *sample_ids,
                                       void *data)
{
        return NX_TRUE;

        // unused
        (void)sample_ids;
        (void)data;
}

int nx_usac_fit_homography(int *n_max_models, double **models,
                           const int *sample_ids,
                           void *data)
{
        struct NXUSACHomographyData *hdata = (struct NXUSACHomographyData *)data;
        double det = nx_homography_estimate_4pt(models[0],
                                                sample_ids,
                                                hdata->corr_list);
        if (det == 0.0)
                return 0;
        else
                return 1;

        // unused
        (void)n_max_models;
}

NXBool nx_usac_check_homography(double *model, const int *sample_ids,
                                void *data)
{
        static const double MIN_ANGLE = 15.0 * NX_PI / 180.0;
        const double MAX_ABS_COS = fabs(cos(MIN_ANGLE));

        return nx_homography_check(model, MAX_ABS_COS);

        // unused
        (void)sample_ids;
        (void)data;
}

double nx_usac_score_homography(double *model, const int *sample_ids,
                                void *data)
{
        struct NXUSACHomographyData *hdata = (struct NXUSACHomographyData *)data;

        int n_inliers = nx_homography_mark_inliers(model, hdata->n_corr,
                                                      hdata->corr_list,
                                                      hdata->inlier_tolerance);

        return n_inliers;

        // unused
        (void)sample_ids;
}

NXBool nx_usac_check_homography_degeneracy(double *model, const int *sample_ids,
                                           void *data)
{
        return NX_TRUE;

        // unused
        (void)model;
        (void)sample_ids;
        (void)data;
}

NXBool nx_usac_refine_homography_local(double *new_model, const double *model,
                                     const int *sample_ids, void *data)
{
        return NX_FALSE;

        // unused
        (void)new_model;
        (void)model;
        (void)sample_ids;
        (void)data;
}

void nx_usac_refine_homography_global(double *new_model, const double *model,
                                      void *data)
{
        struct NXUSACHomographyData *hdata = (struct NXUSACHomographyData *)data;

        nx_dvec_copy(9, new_model, model);
        int n_inliers_best = nx_homography_mark_inliers(new_model,
                                                        hdata->n_corr,
                                                        hdata->corr_list,
                                                        hdata->inlier_tolerance);
        int n_inliers = 1;
        while (n_inliers_best > (n_inliers + 5)) {
                nx_homography_estimate_dlt_inliers(new_model,
                                                   hdata->n_corr,
                                                   hdata->corr_list);
                n_inliers = n_inliers_best;
                n_inliers_best = nx_homography_mark_inliers(new_model,
                                                            hdata->n_corr,
                                                            hdata->corr_list,
                                                            hdata->inlier_tolerance);
        }
}

NXBool nx_usac_homography_terminate(const double *best_model,
                                    double best_score, void *data)
{
        const int PROSAC_INC = 1;

        struct NXUSACHomographyData *hdata = (struct NXUSACHomographyData *)data;
        ++hdata->n_iterations;
        hdata->n_top_hypo += PROSAC_INC;
        if (hdata->n_top_hypo >= hdata->n_corr) {
                hdata->n_top_hypo = hdata->n_corr;
        }

        int n_inliers = (int)best_score;
        return hdata->n_iterations >= hdata->max_n_iter
                || n_inliers >= 200;

        // unused
        (void)best_model;
}

int nx_usac_estimate_homography(double *h, int n_corr,
                                struct NXPointMatch2D *corr_list,
                                double inlier_tolerance, int max_n_iter)
{
        const int PROSAC_START = 10;

        if (n_corr < 4)
            return 0;

        qsort((void *)corr_list, n_corr, sizeof(struct NXPointMatch2D),
              nx_point_match_2d_cmp_match_cost);

        struct NXUSACHomographyData data;
        data.n_corr = n_corr;
        data.corr_list = corr_list;
        data.inlier_tolerance = inlier_tolerance;
        data.max_n_iter = max_n_iter;
        data.n_iterations = 0;
        data.n_top_hypo = PROSAC_START;
        if (data.n_top_hypo > n_corr)
            data.n_top_hypo = n_corr;

        struct NXUSACConfig config;
        config.sampling_fn = nx_usac_homography_sampling;
        config.sample_check_fn = nx_usac_check_homography_sample;
        config.model_fit_fn = nx_usac_fit_homography;
        config.model_check_fn = nx_usac_check_homography;
        config.model_score_fn = nx_usac_score_homography;
        config.degeneracy_check_fn = nx_usac_check_homography_degeneracy;
        config.local_refinement_fn = nx_usac_refine_homography_local;
        config.global_refinement_fn = nx_usac_refine_homography_global;
        config.termination_fn = nx_usac_homography_terminate;
        nx_usac(9, h, 4, &config, (void *)&data);

        int n_inliers = nx_homography_mark_inliers(h, n_corr,
                                                   corr_list, inlier_tolerance);

        // last check for homography, return zero inliers if fails.
        if (!nx_usac_check_homography(h, NULL, &data))
                return 0;
        else
                return n_inliers;
}
