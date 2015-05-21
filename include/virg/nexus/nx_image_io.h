/**
 * @file nx_image_io.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2015 Mustafa Ozuysal. All rights reserved.
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

#include "virg/nexus/nx_config.h"

#include "virg/nexus/nx_image_io_params.h"
#include "virg/nexus/nx_image_io_pnm.h"
#include "virg/nexus/nx_image_io_jpeg.h"

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
