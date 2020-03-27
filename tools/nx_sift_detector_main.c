/**
 * @file nx_sift_detector_main.c
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
#include "virg/nexus/nx_options.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_image_io.h"
#include "virg/nexus/nx_sift_detector.h"

int main(int argc, char **argv)
{
        struct NXOptions* opt = nx_options_alloc();
        nx_options_add(opt, "S", "-i", "filename of the IMAGE to use", NULL);
        nx_options_add(opt, "s", "-o", "output filename to save keypoints and descriptors", "sift.bin");
        nx_sift_parameters_add_to_options(opt);
        nx_options_add(opt, "b", "-v|--verbose", "display more information", NX_FALSE);
        nx_options_add_help(opt);
        nx_options_set_usage_header(opt, "Detects DoG keypoints and computes the SIFT descriptors.\n\n");
        nx_options_set_usage_footer(opt, "\nCopyright (C) 2020 Mustafa Ozuysal.\n");

        nx_options_set_from_args(opt, argc, argv);
        NXBool is_verbose = nx_options_get_bool(opt, "-v");
        const char *input_name = nx_options_get_string(opt, "-i");
        const char *output_name = nx_options_get_string(opt, "-o");

        if (is_verbose)
                nx_options_print_values(opt, stderr);

        struct NXSIFTDetectorParams params =  nx_sift_parameters_from_options(opt);
        struct NXSIFTDetector *sift_detector = nx_sift_detector_new(params);
        struct NXImage* img = nx_image_alloc();

        nx_image_xload(img, input_name, NX_IMAGE_LOAD_GRAYSCALE);
        if (is_verbose)
                NX_LOG("SIFT", "Loaded image of size %dx%d from file %s.",
                       img->width, img->height, input_name);

        int max_n_keys = 100;
        struct NXKeypoint *keys = NX_NEW(max_n_keys, struct NXKeypoint);
        uchar *desc = NX_NEW_UC(max_n_keys * NX_SIFT_DESC_DIM);
        int n_keys = nx_sift_detector_compute(sift_detector, img,
                                              &max_n_keys, &keys, &desc);

        if (is_verbose)
                NX_LOG("SIFT", "detected %d keys", n_keys);

        printf("%10s,%7s,%7s,%5s,%7s,%7s\n",
               "score", "x","y","scale", "ori", "sigma");
        for (int i = 0; i < n_keys; ++i) {
                const struct NXKeypoint *key = keys + i;
                printf("%10.6f,%7.2f,%7.2f,%5.2f,%7.4f,%7.4f\n",
                       key->score, key->xs, key->ys,
                       key->scale, key->ori, key->sigma);
        }
        nx_sift_xsave(output_name, n_keys, keys, desc);

        nx_free(desc);
        nx_free(keys);
        nx_sift_detector_free(sift_detector);
        nx_image_free(img);
        nx_options_free(opt);

        return EXIT_SUCCESS;
}
