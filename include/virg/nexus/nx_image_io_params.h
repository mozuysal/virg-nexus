/**
 * @file nx_image_io_params.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2014 Izmir Institute of Technology. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_IMAGE_IO_PARAMS_H
#define VIRG_NEXUS_NX_IMAGE_IO_PARAMS_H

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

enum NXImageLoadMode {
        NX_IMAGE_LOAD_AS_IS = -1,
        NX_IMAGE_LOAD_GRAYSCALE = 0,
        NX_IMAGE_LOAD_RGBA
};

__NX_END_DECL

#endif
