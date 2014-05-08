/**
 * @file nx_image.h
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
#ifndef VIRG_NEXUS_NX_IMAGE_H
#define VIRG_NEXUS_NX_IMAGE_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_image_warp.h"

__NX_BEGIN_DECL

struct NXMemBlock;

enum NXImageType {
        NX_IMAGE_GRAYSCALE = 0,
        NX_IMAGE_RGBA
};

struct NXImage
{
        int width;
        int height;
        enum NXImageType type;
        int n_channels;

        struct NXMemBlock *mem;
        uchar *data;
        int row_stride;
};

static inline int nx_image_n_channels(enum NXImageType type);

struct NXImage *nx_image_alloc();

struct NXImage *nx_image_new(int width, int height, enum NXImageType type);

struct NXImage *nx_image_new_gray(int width, int height);

struct NXImage *nx_image_new_rgba(int width, int height);

void nx_image_free(struct NXImage *img);

void nx_image_resize(struct NXImage *img, int width, int height,
                     int row_stride, enum NXImageType type);

void nx_image_release(struct NXImage *img);

struct NXImage *nx_image_copy0(const struct NXImage *img);

void nx_image_copy(struct NXImage *dest, const struct NXImage *src);

void nx_image_set_zero(struct NXImage *img);

void nx_image_wrap(struct NXImage *img, uchar *data, int width, int height,
                   int row_stride, enum NXImageType type, NXBool own_memory);

void nx_image_sub_rect_of(struct NXImage *sub_rect, struct NXImage *img, int tl_x, int tl_y, int rect_width, int rect_height);

void nx_image_swap(struct NXImage *img0, struct NXImage *img1);

void nx_image_convert(struct NXImage *dest, const struct NXImage *src);

void nx_image_scale(struct NXImage *dest, const struct NXImage *src, float scale_f);

void nx_image_downsample(struct NXImage *dest, const struct NXImage *src);

void nx_image_downsample_aa_x(struct NXImage *dest, const struct NXImage *src);

void nx_image_downsample_aa_y(struct NXImage *dest, const struct NXImage *src);

uchar *nx_image_filter_buffer_alloc(int width, int height, float sigma_x, float sigma_y);

void nx_image_smooth_si(struct NXImage *dest, const struct NXImage *src,
                        float sigma_x, float sigma_y, uchar *filter_buffer);

void nx_image_smooth_s(struct NXImage *dest, const struct NXImage *src,
                       float sigma_x, float sigma_y, uchar *filter_buffer);

void nx_image_transform_affine(struct NXImage *dest, const struct NXImage *src,
                               const float *t_src2dest, enum NXImageWarpBackgroundMode bg_mode);

void nx_image_transform_affine_prm(struct NXImage *dest, const struct NXImage *src,
                                   float lambda, float psi, float theta, float phi,
                                   enum NXImageWarpBackgroundMode bg_mode);

uchar nx_image_sample_bilinear(const struct NXImage *img, float xs, float ys);

#define NX_IMAGE_ASSERT_GRAYSCALE(img) \
        do {                               \
                NX_ASSERT_CUSTOM("Image must be grayscale",img->type == NX_IMAGE_GRAYSCALE); \
        } while(0)

#define NX_IMAGE_ASSERT_EQUAL_TYPES(img0,img1)  \
        do {                               \
                NX_ASSERT_CUSTOM("Images must be of the same type",img0->type == img1->type); \
        } while(0)

__NX_END_DECL

int nx_image_n_channels(enum NXImageType type)
{
        return type == NX_IMAGE_GRAYSCALE ? 1 : 4;
}

#endif
