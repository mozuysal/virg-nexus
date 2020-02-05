/**
 * @file nx_stitch_main.c
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
 * along with VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include <stdlib.h>

#include "virg/nexus/nx_options.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_image_io.h"

#include "nx_panorama_builder.h"

#define LOG_TAG "Stitcher"

int main(int argc, char **argv)
{
        struct NXOptions *opt = nx_options_alloc();

        nx_panorama_builder_add_options(opt);
        nx_options_add_help(opt);
        nx_options_set_from_args(opt, argc, argv);

        nx_options_print_values(opt, stderr);

        struct NXPanoramaBuilder *builder = NULL;
        builder = nx_panorama_builder_new_from_options(opt);

        nx_panorama_builder_run(builder);

        const struct NXImage *panorama = nx_panorama_builder_get_panorama(builder);
        nx_image_xsave(panorama, "/tmp/panorama.jpeg");
        NX_LOG(NX_LOG_TAG, "Saved panorama as /tmp/panorama.jpeg");

        nx_panorama_builder_free(builder);
        nx_options_free(opt);

        return EXIT_SUCCESS;
}

