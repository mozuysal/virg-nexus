/**
 * @file nx_image_io_png.h
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
#ifndef VIRG_NEXUS_NX_IMAGE_IO_PNG_H
#define VIRG_NEXUS_NX_IMAGE_IO_PNG_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_image_io_params.h"

__NX_BEGIN_DECL

void nx_image_xsave_png(const struct NXImage *img, const char *filename);

void nx_image_xload_png(struct NXImage *img, const char *filename, enum NXImageLoadMode mode);

NXResult nx_image_save_png(const struct NXImage *img, const char *filename);

NXResult nx_image_load_png(struct NXImage *img, const char *filename, enum NXImageLoadMode mode);

__NX_END_DECL

#endif