/**
 * @file nx_temple_color_128.c
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
 * You should have received a copy of the GNU General Public License along with
 * VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/nx_temple_color_128.h"

#include <stdio.h>
#include <string.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_string.h"
#include "virg/nexus/nx_io.h"
#include "virg/nexus/nx_filesystem.h"

#define NX_TEMPLE_COLOR_128_SEQUENCE_DIR_FORMAT "%s/%s"
#define NX_TEMPLE_COLOR_128_ATTRIB_FILE_FORMAT "%s/%s/%s_att.txt"
#define NX_TEMPLE_COLOR_128_FRAMES_FILE_FORMAT "%s/%s/%s_frames.txt"
#define NX_TEMPLE_COLOR_128_GT_FILE_FORMAT "%s/%s/%s_gt.txt"
#define NX_TEMPLE_COLOR_128_FRAME_FILE_FORMAT "%s/%s/img/%04d.jpg"

#define NX_TEMPLE_COLOR_128_N_ATTRIBUTES 11

static const char *NX_TEMPLE_COLOR_128_ATTRIBUTES[NX_TEMPLE_COLOR_128_N_ATTRIBUTES] = {
        "VI", "SV", "OCC", "DEF", "MB", "FM", "IPR", "OPR", "OV", "BC", "LR" };

static void nx_temple_color_128_xread_frame_range(const char *base_dir,
                                                  const char *seq_name,
                                                  int *start,
                                                  int *end)
{
        char *frames_file = nx_fstr(NX_TEMPLE_COLOR_128_FRAMES_FILE_FORMAT,
                                    base_dir, seq_name, seq_name);
        FILE *fin = nx_xfopen(frames_file, "r");
        if (2 != fscanf(fin, "%d,%d", start, end))
                NX_FATAL(NX_LOG_TAG, "Error reading frame range from file %s",
                         frames_file);
        nx_xfclose(fin, frames_file);
        nx_free(frames_file);
}

static uint16_t nx_temple_color_128_get_attr(FILE *fin)
{
        const size_t BUFFER_SIZE = 8;
        char buffer[BUFFER_SIZE];
        if (NULL == fgets(&buffer[0], BUFFER_SIZE, fin))
                return 0;

        for (int i = 0; i < NX_TEMPLE_COLOR_128_N_ATTRIBUTES; ++i)
                if (0 == strncmp(NX_TEMPLE_COLOR_128_ATTRIBUTES[i], buffer,
                                 strlen(NX_TEMPLE_COLOR_128_ATTRIBUTES[i])))
                        return 1 << i;
        return 0;
}

static uint16_t
nx_temple_color_128_xread_attributes(const char *base_dir,
                                     const char *seq_name)
{
        char *attrib_file = nx_fstr(NX_TEMPLE_COLOR_128_ATTRIB_FILE_FORMAT,
                                    base_dir, seq_name, seq_name);
        FILE *fin = nx_xfopen(attrib_file, "r");

        uint16_t attr = 0;
        uint16_t fattr = 0;
        do {
                fattr = nx_temple_color_128_get_attr(fin);
                attr |= fattr;
        } while (fattr != 0);

        nx_xfclose(fin, attrib_file);
        nx_free(attrib_file);

        return attr;
}

static float **
nx_temple_color_128_xread_ground_truth(const char *base_dir,
                                       const char *seq_name,
                                       int length)
{
        float **gt = NX_NEW(length, float*);
        for (int i = 0; i < length; ++i) {
                gt[i] = NX_NEW_S(4);
        }

        char *gt_file = nx_fstr(NX_TEMPLE_COLOR_128_GT_FILE_FORMAT,
                                base_dir, seq_name, seq_name);
        FILE *fin = nx_xfopen(gt_file, "r");

        for (int i = 0; i < length; ++i) {
                if (4 != fscanf(fin, "%f,%f,%f,%f\n",
                                gt[i], gt[i] + 1, gt[i] + 2, gt[i] + 3))
                        NX_FATAL(NX_LOG_TAG,
                                 "Error reading Temple Color 128 ground truth from file %s line %d",
                                 gt_file, i+1);
        }

        nx_xfclose(fin, gt_file);
        nx_free(gt_file);

        return gt;
}

struct NXTempleColor128Sequence *
nx_temple_color_128_sequence_new(const char *base_dir,
                                 const char *seq_name)
{
        NX_ASSERT_PTR(base_dir);
        NX_ASSERT_PTR(seq_name);

        if (!nx_check_dir(base_dir))
                NX_FATAL(NX_LOG_TAG, "Temple Color 128 base directory %s does not exist", base_dir);

        NXBool found = NX_FALSE;
        for (int i = 0; i < NX_TEMPLE_COLOR_128_N_SEQ; ++i) {
                size_t len = strlen(NX_TEMPLE_COLOR_128_SEQ_NAMES[i]);
                if (0 == strncmp(seq_name, NX_TEMPLE_COLOR_128_SEQ_NAMES[i], len))
                        found = NX_TRUE;
        }
        if (!found) {
                NX_FATAL(NX_LOG_TAG, "%s is not a standard Temple Color 128 sequence",
                         seq_name);
        }

        char *sequence_dir = nx_fstr(NX_TEMPLE_COLOR_128_SEQUENCE_DIR_FORMAT,
                                     base_dir, seq_name);
        if (!nx_check_dir(sequence_dir))
                NX_FATAL(NX_LOG_TAG, "Temple Color 128 sequence directory %s does not exist", sequence_dir);
        nx_free(sequence_dir);

        struct NXTempleColor128Sequence *seq = NX_NEW(1, struct NXTempleColor128Sequence);
        seq->base_dir = nx_strdup(base_dir);
        seq->name = nx_strdup(seq_name);

        nx_temple_color_128_xread_frame_range(base_dir, seq_name,
                                              &seq->start_frame,
                                              &seq->end_frame);
        seq->length = seq->end_frame - seq->start_frame + 1;

        seq->attributes = nx_temple_color_128_xread_attributes(base_dir,
                                                               seq_name);
        seq->ground_truth = nx_temple_color_128_xread_ground_truth(base_dir,
                                                                   seq_name,
                                                                   seq->length);
        return seq;
}

void nx_temple_color_128_sequence_free(struct NXTempleColor128Sequence *seq)
{
        if (seq) {
                nx_free(seq->base_dir);
                nx_free(seq->name);
                for (int i = 0; i < seq->length; ++i)
                        nx_free(seq->ground_truth[i]);
                nx_free(seq->ground_truth);
                nx_free(seq);
        }
}

void nx_temple_color_128_sequence_print(struct NXTempleColor128Sequence *seq)
{
        NX_ASSERT_PTR(seq);

        printf("Temple Color 128 Sequence %s/%s:\n", seq->base_dir, seq->name);
        printf("   Frame Range %d to %d (%d)\n", seq->start_frame, seq->end_frame, seq->length);
        printf("   Attributes: ");
        for (int i = 0; i < NX_TEMPLE_COLOR_128_N_ATTRIBUTES; ++i)
                if (seq->attributes & (1 << i))
                        printf("%s", NX_TEMPLE_COLOR_128_ATTRIBUTES[i]);
        printf("\n");
}

void nx_temple_color_128_sequence_xload_frame(const struct NXTempleColor128Sequence *seq,
                                              int frame_id,
                                              struct NXImage *img,
                                              enum NXImageLoadMode mode)
{
        NX_ASSERT_PTR(seq);
        NX_ASSERT_PTR(img);
        NX_ASSERT_RANGE(frame_id, seq->start_frame, seq->end_frame);

        char *filename = nx_fstr(NX_TEMPLE_COLOR_128_FRAME_FILE_FORMAT,
                                 seq->base_dir,
                                 seq->name,
                                 frame_id);
        nx_image_xload(img, filename, mode);
        nx_free(filename);
}
