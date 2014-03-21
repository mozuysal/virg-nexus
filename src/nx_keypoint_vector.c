/**
 * @file nx_keypoint_vector.c
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
#include "virg/nexus/nx_keypoint_vector.h"

#define NX_VECTOR_NAME NXKeypointVector
#define NX_VECTOR_FUNC(fn) nx_keypoint_vector_##fn
#define NX_VECTOR_ITEM struct NXKeypoint

#include "nx_vector_gen.c"

#undef NX_VECTOR_NAME
#undef NX_VECTOR_FUNC
#undef NX_VECTOR_ITEM
