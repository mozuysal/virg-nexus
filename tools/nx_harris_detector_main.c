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

        NXBool is_verbose = nx_options_get_bool(opt, "-v");

        if (is_verbose)
                nx_options_print_values(opt, stderr);

        const char* input_name = nx_options_get_string(opt, "-i");
        struct NXImage *img = nx_image_alloc();
        nx_image_xload(img, input_name, NX_IMAGE_LOAD_GRAYSCALE);

        if (is_verbose)
                NX_LOG("HARRIS", "Loaded image of size %dx%d from file %s.",
                       img->width, img->height, input_name);

        nx_image_free(img);
        nx_options_free(opt);

        return EXIT_SUCCESS;
}
