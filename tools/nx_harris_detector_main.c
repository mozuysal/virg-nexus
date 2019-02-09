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

int main(int argc, char** argv)
{
        struct NXOptions* opt = nx_options_new("ddb",
                                               "-w|--sigma_win", "integration scale", 3.0,
                                               "-k", "Harris corner score parameter", 0.06,
                                               "-v|--verbose", "enable more information", NX_FALSE);
        nx_options_add_help(opt);

        nx_options_set_from_args(opt, argc, argv);

        NXBool is_verbose = nx_options_get_bool(opt, "-v");

        if (is_verbose)
                nx_options_print_values(opt, stderr);

        nx_options_free(opt);

        return EXIT_SUCCESS;
}
