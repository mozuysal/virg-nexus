/**
 * @file nx_float_image.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Izmir Institute of Technology. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_FLOAT_IMAGE_H
#define VIRG_NEXUS_NX_FLOAT_IMAGE_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_image_warp.h"

__NX_BEGIN_DECL

struct NXMemBlock;
struct NXImage;

struct NXFloatImage
{
        int width;
        int height;
        int n_channels;

        struct NXMemBlock *mem;
        float *data;
        int row_stride;
};

struct NXFloatImage *nx_float_image_alloc();

struct NXFloatImage *nx_float_image_new(int width, int height, int n_channels);

struct NXFloatImage *nx_float_image_new_gray(int width, int height);

struct NXFloatImage *nx_float_image_new_rgba(int width, int height);

void nx_float_image_free(struct NXFloatImage *fimg);

void nx_float_image_resize(struct NXFloatImage *fimg, int width, int height,
                           int n_channels, int row_stride);

void nx_float_image_release(struct NXFloatImage *fimg);

struct NXFloatImage *nx_float_image_copy0(const struct NXFloatImage *fimg);

void nx_float_image_copy(struct NXFloatImage *dest, const struct NXFloatImage *src);

void nx_float_image_set_zero(struct NXFloatImage *fimg);

void nx_float_image_wrap(struct NXFloatImage *fimg, float *data, int width, int height,
                         int n_channels, int row_stride, NXBool own_memory);

void nx_float_image_swap(struct NXFloatImage *fimg0, struct NXFloatImage *fimg1);

float *nx_float_image_filter_buffer_alloc(int width, int height, float sigma_x, float sigma_y);

void nx_float_image_smooth_s(struct NXFloatImage *dest, const struct NXFloatImage *src,
                             float sigma_x, float sigma_y, float *filter_buffer);

void nx_float_image_transform_affine(struct NXImage *dest, const struct NXFloatImage *src_coeff,
                                     const float *t_src2dest, enum NXImageWarpBackgroundMode bg_mode);

void nx_float_image_transform_affine_prm(struct NXImage *dest, const struct NXFloatImage *src_coeff,
                                         float lambda, float psi, float theta, float phi,
                                         enum NXImageWarpBackgroundMode bg_mode);

void nx_float_image_xsave_raw(const struct NXFloatImage *fimg, const char *filename);

void nx_float_image_xload_raw(struct NXFloatImage *fimg, const char *filename);

NXResult nx_float_image_save_raw(const struct NXFloatImage *fimg, const char *filename);

NXResult nx_float_image_load_raw(struct NXFloatImage *fimg, const char *filename);

struct NXFloatImage *nx_float_image_from_uchar0(const struct NXImage *img);

void nx_float_image_from_uchar(struct NXFloatImage *fimg, const struct NXImage *img);

void nx_float_image_to_uchar(struct NXImage *img, const struct NXFloatImage *fimg);

void nx_float_image_spline_coeff_of(struct NXFloatImage *fimg, const struct NXImage *img);

#define NX_FLOAT_IMAGE_ASSERT_GRAYSCALE(fimg)                            \
        do {                                                            \
                NX_ASSERT_CUSTOM("Float image must have a single channel", fimg->n_channels == 1); \
        } while(0)

#define NX_FLOAT_IMAGE_ASSERT_EQUAL_N_CHANNELS(fimg0,fimg1)               \
        do {                                                            \
                NX_ASSERT_CUSTOM("Float images must have the same number of channels", fimg0->n_channels == fimg1->n_channels); \
        } while(0)

__NX_END_DECL

#endif
