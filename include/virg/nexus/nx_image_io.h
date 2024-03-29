/**
 * @file nx_image_io.h
 *
 * Copyright (C) 2019,2020 Mustafa Ozuysal. All rights reserved.
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
 * VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_IMAGE_IO_H
#define VIRG_NEXUS_NX_IMAGE_IO_H

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_image_io_params.h"
#include "virg/nexus/nx_image_io_pnm.h"
#include "virg/nexus/nx_image_io_jpeg.h"
#include "virg/nexus/nx_image_io_png.h"

enum NXImageFileFormat {
        NX_IMAGE_FILE_FORMAT_UNKNOWN = 0,
        NX_IMAGE_FILE_FORMAT_PNM_ASCII,
        NX_IMAGE_FILE_FORMAT_PNM_BINARY,
        NX_IMAGE_FILE_FORMAT_JPEG,
        NX_IMAGE_FILE_FORMAT_PNG
};

enum NXImageFileFormat nx_image_file_format_from_header(const char *filename);
enum NXImageFileFormat nx_image_file_format_from_filename(const char *filename);

void nx_image_xsave(const struct NXImage *img, const char *filename);

void nx_image_xload(struct NXImage *img, const char *filename, enum NXImageLoadMode mode);

NXResult nx_image_save(const struct NXImage *img, const char *filename);

NXResult nx_image_load(struct NXImage *img, const char *filename, enum NXImageLoadMode mode);

__NX_END_DECL

#endif
