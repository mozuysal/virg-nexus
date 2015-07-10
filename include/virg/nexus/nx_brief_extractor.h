/**
 * @file nx_brief_extractor.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_BRIEF_EXTRACTOR_H
#define VIRG_NEXUS_NX_BRIEF_EXTRACTOR_H

#include <stdio.h>

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"

__NX_BEGIN_DECL

#define NX_BRIEF_EXTRACTOR_GOOD_SEED_N32_R16 1431142416U
#define NX_BRIEF_EXTRACTOR_GOOD_SEED_N32_R24 1431228807U

struct NXImagePyr;

struct NXBriefExtractor {
        int radius;
        int n_octets;

        int *offsets;
        int offset_limits[4];

        int pyr_level_offset;
};

struct NXBriefExtractor *nx_brief_extractor_alloc();

struct NXBriefExtractor *nx_brief_extractor_new(int n_octets, int radius);

struct NXBriefExtractor *nx_brief_extractor_new_with_seed(int n_octets, int radius, uint32_t seed);

void nx_brief_extractor_free(struct NXBriefExtractor *be);

void nx_brief_extractor_resize(struct NXBriefExtractor *be, int n_octets, int radius);

void nx_brief_extractor_randomize_with_seed(struct NXBriefExtractor *be, uint32_t seed);

void nx_brief_extractor_randomize(struct NXBriefExtractor *be);

void nx_brief_extractor_update_limits(struct NXBriefExtractor *be);

NXBool nx_brief_extractor_check_point_pyr(struct NXBriefExtractor *be, const struct NXImagePyr *pyr, int x, int y, int level);
void nx_brief_extractor_compute_pyr(struct NXBriefExtractor *be, const struct NXImagePyr *pyr, int x, int y, int level, uchar *desc);

NXBool nx_brief_extractor_compute_pyr_at_theta(struct NXBriefExtractor *be, const struct NXImagePyr *pyr, int x, int y, int level, float theta, uchar *desc);

int nx_brief_extractor_descriptor_distance(int n_octets, const uchar *desc0, const uchar *desc1);

NXResult nx_brief_extractor_write(const struct NXBriefExtractor *be, FILE *stream);
NXResult nx_brief_extractor_read(struct NXBriefExtractor *be, FILE *stream);

void nx_brief_extractor_xwrite(const struct NXBriefExtractor *be, FILE *stream);
void nx_brief_extractor_xread(struct NXBriefExtractor *be, FILE *stream);

__NX_END_DECL

#endif
