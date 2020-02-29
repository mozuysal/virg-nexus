/**
 * @file nx_image.h
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
#ifndef VIRG_NEXUS_NX_IMAGE_H
#define VIRG_NEXUS_NX_IMAGE_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/nx_types.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_colorspace.h"

__NX_BEGIN_DECL

struct NXMemBlock;

enum NXImageType {
        NX_IMAGE_GRAYSCALE = 0,
        NX_IMAGE_RGBA
};

enum NXImageDataType {
        NX_IMAGE_UCHAR = 0,
        NX_IMAGE_FLOAT32
};

static const int NX_IMAGE_STRIDE_DEFAULT = -1;

struct NXImage
{
        int width;
        int height;
        enum NXImageType type;
        enum NXImageDataType dtype;
        int n_channels;

        struct NXMemBlock *mem;
        union {
                void *v;
                uchar *uc;
                float *f32;
        } data;
        int row_stride;
};

struct NXImage *nx_image_alloc();

struct NXImage *nx_image_new(int width, int height,
                             enum NXImageType type, enum NXImageDataType dtype);

struct NXImage *nx_image_new_gray(int width, int height,
                                  enum NXImageDataType dtype);

struct NXImage *nx_image_new_rgba(int width, int height,
                                  enum NXImageDataType dtype);

struct NXImage *nx_image_new_gray_uc(int width, int height);

struct NXImage *nx_image_new_rgba_uc(int width, int height);

struct NXImage *nx_image_new_gray_f32(int width, int height);

struct NXImage *nx_image_new_rgba_f32(int width, int height);

/**
 * Creates and returns a new image with the same width, height, type, and dtype
 * as the source image. The row_stride of the created image does not necessarily
 * match the source.
 *
 */
struct NXImage *nx_image_new_like(const struct NXImage *src);

void nx_image_free(struct NXImage *img);

#define NX_IMAGE_ASSERT_GRAYSCALE(img) \
        do {                               \
                NX_ASSERT_CUSTOM("Image must be of type grayscale",img->type == NX_IMAGE_GRAYSCALE); \
        } while(0)

#define NX_IMAGE_ASSERT_RGBA(img) \
        do {                               \
                NX_ASSERT_CUSTOM("Image must be of type RGBA",img->type == NX_IMAGE_RGBA); \
        } while(0)

#define NX_IMAGE_ASSERT_UCHAR(img) \
        do {                               \
                NX_ASSERT_CUSTOM("Image data type must be unsigned char",img->dtype == NX_IMAGE_UCHAR); \
        } while(0)

#define NX_IMAGE_ASSERT_FLOAT32(img)       \
        do {                               \
                NX_ASSERT_CUSTOM("Image data type must be single-precision floating point",img->dtype == NX_IMAGE_FLOAT32); \
        } while(0)

#define NX_IMAGE_ASSERT_GRAYSCALE_UCHAR(img)    \
        do {                               \
                NX_ASSERT_CUSTOM("Image must be grayscale and its data type must be uchar",img->type == NX_IMAGE_GRAYSCALE && img->dtype == NX_IMAGE_UCHAR); \
        } while(0)

#define NX_IMAGE_ASSERT_GRAYSCALE_FLOAT32(img)    \
        do {                               \
                NX_ASSERT_CUSTOM("Image must be grayscale and its data type must be single-precision floating point",img->type == NX_IMAGE_GRAYSCALE && img->dtype == NX_IMAGE_FLOAT32); \
        } while(0)

#define NX_IMAGE_ASSERT_EQUAL_TYPES_AND_DTYPES(img0,img1)  \
        do {                               \
                NX_ASSERT_CUSTOM("Images must be of the same type and dtype",img0->type == img1->type && img0->dtype == img1->dtype); \
        } while(0)

static inline int nx_image_n_channels(enum NXImageType type)
{
        switch (type) {
        case NX_IMAGE_GRAYSCALE: return 1;
        case NX_IMAGE_RGBA: return 4;
        default:
                NX_FATAL(NX_LOG_TAG,"Unhandled switch case for image type!");
        }
}

static inline int nx_image_bytes_per_channel(enum NXImageDataType dtype)
{
        switch (dtype) {
        case NX_IMAGE_UCHAR: return sizeof(uchar);
        case NX_IMAGE_FLOAT32: return sizeof(float);
        default:
                NX_FATAL(NX_LOG_TAG,"Unhandled switch case for image data type!");
        }
}

void nx_image_resize(struct NXImage *img, int width, int height,
                     int row_stride, enum NXImageType type,
                     enum NXImageDataType dtype);

/**
 * Resizes the given image to the same width, height, type, and dtype of the
 * source image. The row_stride of the resized image does not necessarily match
 * the source.
 *
 */
void nx_image_resize_like(struct NXImage *img, const struct NXImage *src);

void nx_image_release(struct NXImage *img);

struct NXImage *nx_image_copy0(const struct NXImage *img);

void nx_image_copy(struct NXImage *dest, const struct NXImage *src);

void nx_image_convert_type(struct NXImage* img, enum NXImageType type);

void nx_image_convert_dtype(struct NXImage* dest, const struct NXImage *src);

void nx_image_apply_colormap(struct NXImage* color, struct NXImage* gray,
                             enum NXColorMap map);

void nx_image_set_zero(struct NXImage *img);

void nx_image_normalize_to_zero_one(struct NXImage *img, NXBool symmetric_around_zero);

void nx_image_axpy(struct NXImage *img, float a, float y);

void nx_image_subtract(struct NXImage *difference, const struct NXImage *img0,
                       const struct NXImage *img1);

void nx_image_wrap(struct NXImage *img, void *data, int width, int height,
                   int row_stride, enum NXImageType type,
                   enum NXImageDataType dtype, NXBool own_memory);

void nx_image_swap(struct NXImage *img0, struct NXImage *img1);

void nx_image_scale(struct NXImage *dest, const struct NXImage *src,
                    float scale_f);

void nx_image_downsample(struct NXImage *dest, const struct NXImage *src);

void nx_image_upsample(struct NXImage *dest, const struct NXImage *src);

void nx_image_downsample_aa_x(struct NXImage *dest, const struct NXImage *src);

void nx_image_downsample_aa_y(struct NXImage *dest, const struct NXImage *src);

float *nx_image_filter_buffer_alloc(int width, int height,
                                    float sigma_x, float sigma_y,
                                    float kernel_truncation_factor,
                                    int *nk_x, int *nk_y);

void nx_image_smooth(struct NXImage *dest, const struct NXImage *src,
                     float sigma_x, float sigma_y,
                     float kernel_truncation_factor, float *filter_buffer);

void nx_image_filter_box_x(struct NXImage *dest, const struct NXImage *src,
                           int sum_radius, float *filter_buffer);
void nx_image_filter_box_y(struct NXImage *dest, const struct NXImage *src,
                           int sum_radius, float *filter_buffer);
void nx_image_filter_box(struct NXImage *dest, const struct NXImage *src,
                         int sum_radius_x, int sum_radius_y,
                         float *filter_buffer);

void nx_image_deriv_x(struct NXImage *dest, const struct NXImage *src);

void nx_image_deriv_y(struct NXImage *dest, const struct NXImage *src);

__NX_END_DECL

#endif
