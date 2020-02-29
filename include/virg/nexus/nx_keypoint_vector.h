/**
 * @file nx_keypoint_vector.h
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
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
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
