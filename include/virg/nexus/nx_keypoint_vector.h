/**
 * @file nx_keypoint_vector.h
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
#ifndef VIRG_NEXUS_NX_KEYPOINT_VECTOR_H
#define VIRG_NEXUS_NX_KEYPOINT_VECTOR_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_keypoint.h"

#define NX_VECTOR_NAME NXKeypointVector
#define NX_VECTOR_FUNC(fn) nx_keypoint_vector_##fn
#define NX_VECTOR_ITEM struct NXKeypoint

#include "virg/nexus/nx_vector_gen.h"

#undef NX_VECTOR_NAME
#undef NX_VECTOR_FUNC
#undef NX_VECTOR_ITEM

#endif
