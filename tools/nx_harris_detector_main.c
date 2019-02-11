/**
 * @file nx_harris_detector_main.c
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2019 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include <stdlib.h>

#include "virg/nexus/nx_options.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_harris_detector.h"

int main(int argc, char** argv)
{
        struct NXOptions* opt = nx_options_new("Sddb",
                                               "-i", "input IMAGE", "",
                                               "--sigma_int", "integration scale", 3.0,
                                               "-k", "Harris corner score parameter", 0.06,
                                               "-v|--verbose", "log more information to stderr", NX_FALSE);
        nx_options_add_help(opt);
        nx_options_set_usage_header(opt, "Detects Harris corner points.\n\n");
        nx_options_set_usage_footer(opt, "\nCopyright (C) 2019 Mustafa Ozuysal.\n");

        nx_options_set_from_args(opt, argc, argv);
        const char* input_name = nx_options_get_string(opt, "-i");
        float sigma_win = nx_options_get_double(opt, "--sigma_int");
        float k = nx_options_get_double(opt, "-k");
        NXBool is_verbose = nx_options_get_bool(opt, "-v");

        if (is_verbose)
                nx_options_print_values(opt, stderr);

        struct NXImage* img = nx_image_alloc();
        nx_image_xload(img, input_name, NX_IMAGE_LOAD_GRAYSCALE);

        if (is_verbose)
                NX_LOG("HARRIS", "Loaded image of size %dx%d from file %s.",
                       img->width, img->height, input_name);

        struct NXImage** dimg = NX_NEW(3, struct NXImage*);
        for (int i = 0; i < 3; ++i)
                dimg[i] = nx_image_alloc();
        nx_harris_deriv_images(dimg, img, sigma_win);

        struct NXImage* simg = nx_image_alloc();
        nx_harris_score_image(simg, dimg, k);

        int n_keys_max = 1000;
        struct NXKeypoint* keys = NX_NEW(n_keys_max, struct NXKeypoint);
        int n_keys = nx_harris_detect_keypoints(n_keys_max, keys, simg, 0);

        if (is_verbose)
                NX_LOG("HARRIS", "Detected %d keypoints.", n_keys);

        nx_free(keys);
        nx_image_free(simg);
        for (int i = 0; i < 3; ++i)
                nx_image_free(dimg[i]);
        nx_free(dimg);
        nx_image_free(img);
        nx_options_free(opt);

        return EXIT_SUCCESS;
}
