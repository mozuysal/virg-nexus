/**
 * @file nx_vgg_affine_dataset.c
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
#include "virg/nexus/nx_vgg_affine_dataset.h"

#include <stdio.h>
#include <string.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_string.h"
#include "virg/nexus/nx_io.h"
#include "virg/nexus/nx_filesystem.h"
#include "virg/nexus/nx_mat234.h"

const char *NX_VGG_AFFINE_SEQ_NAMES[NX_VGG_AFFINE_N_SEQ] = {
        "bark", "bikes", "boat", "graf", "leuven", "trees", "ubc", "wall"};

#define NX_VGG_AFFINE_SEQUENCE_DIR_FORMAT "%s/%s"
#define NX_VGG_AFFINE_H_FILE_FORMAT "%s/%s/H1to%dp"
#define NX_VGG_AFFINE_FRAME_FILE_FORMAT "%s/%s/img%d.ppm"
#define NX_VGG_AFFINE_FRAME_FILE_ALT_FORMAT "%s/%s/img%d.pgm"

double **
nx_vgg_affine_xread_ground_truth(const char *base_dir,
                                 const char *seq_name,
                                 int length)
{
        double **h = NX_NEW(length, double*);
        h[0] = NX_NEW_D(9);
        nx_dmat3_eye(h[0]);
        for (int i = 1; i < length; ++i) {
                h[i] = NX_NEW_D(9);

                char *h_file = nx_fstr(NX_VGG_AFFINE_H_FILE_FORMAT,
                                       base_dir, seq_name, i+1);
                FILE *fin = nx_xfopen(h_file, "r");

                for (int r= 0; r < 3; ++r) {
                        for (int c= 0; c < 3; ++c) {
                                if (1 != fscanf(fin, "%lf", h[i] + c*3 + r))
                                        NX_FATAL(NX_LOG_TAG,
                                                 "Error reading VGG Affine ground truth from file %s row %d, column %d",
                                                 h_file, r, c);
                        }
                }

                nx_xfclose(fin, h_file);
                nx_free(h_file);
        }

        return h;
}

struct NXVGGAffineSequence *
nx_vgg_affine_sequence_new(const char *base_dir,
                           const char *seq_name)
{
        NX_ASSERT_PTR(base_dir);
        NX_ASSERT_PTR(seq_name);

        if (!nx_check_dir(base_dir))
                NX_FATAL(NX_LOG_TAG, "VGG Affine base directory %s does not exist", base_dir);

        NXBool found = NX_FALSE;
        for (int i = 0; i < NX_VGG_AFFINE_N_SEQ; ++i) {
                size_t len = strlen(NX_VGG_AFFINE_SEQ_NAMES[i]);
                if (0 == strncmp(seq_name, NX_VGG_AFFINE_SEQ_NAMES[i], len))
                        found = NX_TRUE;
        }
        if (!found) {
                NX_FATAL(NX_LOG_TAG, "%s is not a standard VGG Affine sequence",
                         seq_name);
        }

        char *sequence_dir = nx_fstr(NX_VGG_AFFINE_SEQUENCE_DIR_FORMAT,
                                     base_dir, seq_name);
        if (!nx_check_dir(sequence_dir))
                NX_FATAL(NX_LOG_TAG, "VGG Affine sequence directory %s does not exist",
                         sequence_dir);
        nx_free(sequence_dir);

        struct NXVGGAffineSequence *seq = NX_NEW(1, struct NXVGGAffineSequence);
        seq->base_dir = nx_strdup(base_dir);
        seq->name = nx_strdup(seq_name);
        seq->length = 6;
        seq->h = nx_vgg_affine_xread_ground_truth(base_dir, seq_name, seq->length);
        return seq;
}

void nx_vgg_affine_sequence_free(struct NXVGGAffineSequence *seq)
{
        if (seq) {
                nx_free(seq->base_dir);
                nx_free(seq->name);
                for (int i = 0; i < seq->length; ++i)
                        nx_free(seq->h[i]);
                nx_free(seq->h);
                nx_free(seq);
        }
}

void nx_vgg_affine_sequence_xload_frame(const struct NXVGGAffineSequence *seq,
                                        int frame_id,
                                        struct NXImage *img,
                                        enum NXImageLoadMode mode)
{
        NX_ASSERT_PTR(seq);
        NX_ASSERT_PTR(img);
        NX_ASSERT_INDEX(frame_id, seq->length);

        char *filename = nx_fstr(NX_VGG_AFFINE_FRAME_FILE_FORMAT,
                                 seq->base_dir,
                                 seq->name,
                                 frame_id + 1);
        if (NX_FAIL == nx_image_load(img, filename, mode)) {
                nx_free(filename);
                filename = nx_fstr(NX_VGG_AFFINE_FRAME_FILE_ALT_FORMAT,
                                   seq->base_dir,
                                   seq->name,
                                   frame_id + 1);
                nx_image_xload(img, filename, mode);
        }

        nx_free(filename);
}
