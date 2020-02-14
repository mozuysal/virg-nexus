/**
 * @file nx_fit_homography_main.c
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "virg/nexus/nx_options.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_image_io.h"
#include "virg/nexus/nx_sift_detector.h"

struct NXKeyStore {
        int n_keys;
        int max_n_keys;
        struct NXKeypoint *keys;
        uchar *desc;
};

struct NXImage **load_images(struct NXOptions *opt);
struct NXKeyStore *compute_keypoints_and_descriptors(struct NXOptions *opt,
                                                     struct NXImage **imgs);

int main(int argc, char **argv)
{
        struct NXOptions *opt = nx_options_alloc();
        nx_options_set_usage_header(opt, "Fits a homography to the given pair of images. "
                                    "Specify the input pair of images using -l/-r.\n\n");
        nx_options_add(opt, "ss",
                       "-l|--left",  "first image in the pair to match", NULL,
                       "-r|--right", "second image in the pair to match", NULL);
        nx_sift_parameters_add_to_options(opt);
        nx_options_add(opt, "b",
                       "-v|--verbose", "log more information", NX_FALSE);
        nx_options_add_help(opt);
        nx_options_set_from_args(opt, argc, argv);

        NXBool is_verbose = nx_options_get_bool(opt, "-v");
        if (is_verbose)
                nx_options_print_values(opt, stderr);

        struct NXImage **imgs = load_images(opt);
        struct NXKeyStore *stores = compute_keypoints_and_descriptors(opt,
                                                                      imgs);

        nx_free(stores[0].keys);
        nx_free(stores[0].desc);
        nx_free(stores[1].keys);
        nx_free(stores[1].desc);
        nx_free(stores);
        nx_image_free(imgs[0]);
        nx_image_free(imgs[1]);
        nx_free(imgs);
        nx_options_free(opt);

        return EXIT_SUCCESS;
}

struct NXImage **load_images(struct NXOptions *opt)
{
        NX_ASSERT_PTR(opt);

        NXBool is_verbose = nx_options_get_bool(opt, "-v");

        struct NXImage **imgs = NX_NEW(2, struct NXImage *);
        imgs[0] = nx_image_alloc();
        imgs[1] = nx_image_alloc();
        const char *filename_left = nx_options_get_string(opt, "-l");
        const char *filename_right = nx_options_get_string(opt, "-r");
        if (filename_left && filename_right) {
                nx_image_xload(imgs[0], filename_left, NX_IMAGE_LOAD_GRAYSCALE);
                nx_image_xload(imgs[1], filename_right, NX_IMAGE_LOAD_GRAYSCALE);
        } else {
                nx_options_print_usage(opt, stderr);
                NX_FATAL(NX_LOG_TAG, "You must specify the input pair of images!");
        }

        if (is_verbose) {
                NX_LOG(NX_LOG_TAG, "Loaded pair of images:");
                NX_LOG(NX_LOG_TAG, "  (%4d x %4d) from %s", imgs[0]->width, imgs[0]->height, filename_left);
                NX_LOG(NX_LOG_TAG, "  (%4d x %4d) from %s", imgs[1]->width, imgs[1]->height, filename_right);
        }

        return imgs;
}

struct NXKeyStore *
compute_keypoints_and_descriptors(struct NXOptions *opt,
                                  struct NXImage **imgs)
{
        const int INIT_KEY_CAPACITY = 500;

        NXBool is_verbose = nx_options_get_bool(opt, "-v");

        struct NXSIFTDetectorParams sift_param = nx_sift_parameters_from_options(opt);
        struct NXSIFTDetector *detector = nx_sift_detector_new(sift_param);

        struct NXKeyStore *stores = NX_NEW(2, struct NXKeyStore);
        for (int i = 0; i < 2; ++i) {
                stores[i].n_keys = 0;
                stores[i].max_n_keys = INIT_KEY_CAPACITY;
                stores[i].keys = NX_NEW(INIT_KEY_CAPACITY, struct NXKeypoint);
                stores[i].desc = NX_NEW_UC(INIT_KEY_CAPACITY * NX_SIFT_DESC_DIM);

                stores[i].n_keys = nx_sift_detector_compute(detector,
                                                            imgs[i],
                                                            &stores[i].max_n_keys,
                                                            &stores[i].keys,
                                                            &stores[i].desc);

                if (is_verbose)
                        NX_LOG(NX_LOG_TAG,
                               "Detected %d keypoints on the %s image",
                               stores[i].n_keys, i == 0 ? "left" : "right");
        }

        return stores;
}
