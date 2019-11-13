/**
 * @file nx_panorama_builder.c
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
#include "nx_panorama_builder.h"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_string_array.h"
#include "virg/nexus/nx_image_io.h"

void nx_panorama_builder_add_options(struct NXOptions *opt)
{
        nx_options_add(opt,
                       "R",
                       "IMAGES");
}

struct NXPanoramaBuilder
{
        int n_images;
        struct NXStringArray *image_names;
        struct NXImage **images;

        struct NXImage *panorama;
};

struct NXPanoramaBuilder *
nx_panorama_builder_new_from_options(struct NXOptions *opt)
{
        struct NXPanoramaBuilder *builder = NX_NEW(1, struct NXPanoramaBuilder);

        builder->image_names = nx_string_array_new_from_null_terminated_list(nx_options_get_rest(opt));
        builder->n_images = nx_string_array_size(builder->image_names);
        if (builder->n_images < 2) {
                NX_FATAL(NX_LOG_TAG, "Panorama Builder requires at least two images, %d given!",
                         builder->n_images);
        }

        builder->images = NX_NEW(builder->n_images, struct NXImage *);
        for (int i = 0; i < builder->n_images; ++i) {
                builder->images[i] = nx_image_alloc();
                nx_image_xload(builder->images[i],
                               nx_string_array_get(builder->image_names, i),
                               NX_IMAGE_LOAD_AS_IS);
        }

        builder->panorama = nx_image_alloc();

        return builder;
}

void nx_panorama_builder_free(struct NXPanoramaBuilder *builder)
{
        if (builder) {
                nx_string_array_free(builder->image_names);
                for (int i = 0; i < builder->n_images; ++i)
                        nx_image_free(builder->images[i]);
                nx_free(builder->images);
                nx_image_free(builder->panorama);
                nx_free(builder);
        }
}

void nx_panorama_builder_run(struct NXPanoramaBuilder *builder)
{
        NX_ASSERT_PTR(builder);
}

struct NXImage *
nx_panorama_builder_get_panorama(struct NXPanoramaBuilder *builder)
{
        NX_ASSERT_PTR(builder);

        return builder->panorama;
}
