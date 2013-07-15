/**
 * @file nx_image.c
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
#include "virg/nexus/nx_image.h"

#include <stdlib.h>

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_mem_block.h"
#include "virg/nexus/nx_colorspace.h"

struct NXImage *nx_image_alloc()
{
        struct NXImage *img = nx_new(1, struct NXImage);

        img->width = 0;
        img->height = 0;
        img->type = NX_IMAGE_GRAYSCALE;
        img->n_channels = 1;

        img->mem = nx_mem_block_alloc();
        img->data = NULL;
        img->row_stride = 0;

        return img;
}

struct NXImage *nx_image_new(int width, int height, enum NXImageType type)
{
        struct NXImage *img = nx_image_alloc();
        nx_image_resize(img, width, height, type);

        return img;
}

struct NXImage *nx_image_new_gray(int width, int height)
{
        return nx_image_new(width, height, NX_IMAGE_GRAYSCALE);
}

struct NXImage *nx_image_new_rgba(int width, int height)
{
        return nx_image_new(width, height, NX_IMAGE_RGBA);
}

void nx_image_free(struct NXImage *img)
{
        if (img) {
                nx_mem_block_free(img->mem);
                nx_free(img);
        }
}

void nx_image_resize(struct NXImage *img, int width, int height, enum NXImageType type)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT(width > 0);
        NX_ASSERT(height > 0);

        int n_ch = nx_image_n_channels(type);
        size_t length = width * height * n_ch;
        nx_mem_block_resize(img->mem, length);

        img->width = width;
        img->height = height;
        img->type = type;
        img->n_channels = n_ch;
        img->data = img->mem->ptr;
        img->row_stride = width;
}

void nx_image_release(struct NXImage *img)
{
        NX_ASSERT_PTR(img);

        nx_mem_block_release(img->mem);

        img->width = 0;
        img->height = 0;
        img->type = NX_IMAGE_GRAYSCALE;
        img->n_channels = 1;
        img->data = NULL;
        img->row_stride = 0;
}

struct NXImage *nx_image_copy0(const struct NXImage *img)
{
        NX_ASSERT_PTR(img);

        struct NXImage *cpy = nx_image_alloc();
        nx_image_copy(cpy, img);
        return cpy;
}

void nx_image_copy(struct NXImage *dest, const struct NXImage *src)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);

        nx_mem_block_copy(dest->mem, src->mem);

        dest->width = src->width;
        dest->height = src->height;
        dest->type = src->type;
        dest->n_channels = src->n_channels;
        dest->data = dest->mem->ptr;
        dest->row_stride = src->row_stride;
}

void nx_image_set_zero(struct NXImage *img)
{
        NX_ASSERT_PTR(img);

        nx_mem_block_set_zero(img->mem);
}

void nx_image_wrap(struct NXImage *img, uchar *data, int width, int height, int row_stride, enum NXImageType type, NXBool own_memory)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(data);
        NX_ASSERT(row_stride >= width);

        size_t sz = row_stride * height;
        nx_mem_block_wrap(img->mem, data, sz, own_memory);

        img->width = width;
        img->height = height;
        img->type = type;
        img->n_channels = nx_image_n_channels(type);
        img->data = img->mem->ptr;
        img->row_stride = row_stride;
}

void nx_image_swap(struct NXImage *img0, struct NXImage *img1)
{
        NX_ASSERT_PTR(img0);
        NX_ASSERT_PTR(img1);

        nx_mem_block_swap(img0->mem, img1->mem);
        img0->data = img0->mem->ptr;
        img1->data = img1->mem->ptr;

        int tmpi;
        tmpi = img0->width; img1->width = img0->width; img1->width = tmpi;
        tmpi = img0->height; img1->height = img0->height; img1->height = tmpi;
        tmpi = img0->row_stride; img1->row_stride = img0->row_stride; img1->row_stride = tmpi;
        tmpi = img0->n_channels; img1->n_channels = img0->n_channels; img1->n_channels = tmpi;

        enum NXImageType tmpt;
        tmpt = img0->type; img0->type = img1->type; img1->type = tmpt;
}

void nx_image_convert(struct NXImage *dest, const struct NXImage *src)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);

        if (dest->type == src->type) {
                nx_image_copy(dest, src);
                return;
        }

        nx_image_resize(dest, src->width, src->height, dest->type);

        switch (dest->type) {
        case NX_IMAGE_GRAYSCALE:
                nx_convert_rgba_to_gray(dest->width, dest->height,
                                        dest->data, dest->row_stride,
                                        src->data, src->row_stride);
                break;
        case NX_IMAGE_RGBA:
                nx_convert_gray_to_rgba(dest->width, dest->height,
                                        dest->data, dest->row_stride,
                                        src->data, src->row_stride);
                break;
        default:
                nx_fatal("nx_image_convert", "Unknown destination image type %d!", (int)dest->type);
                return;
        }
}

void nx_image_scale(struct NXImage *dest, const struct NXImage *src, float scale_f)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE("nx_image_scale", src);
        NX_IMAGE_ASSERT_GRAYSCALE("nx_image_scale", dest);

        float inv_scale = 1.0f / scale_f;

        for (int y = 0; y < dest->height; ++y) {
                uchar *row = dest->data + y * dest->row_stride;

                float yp = y * inv_scale;
                int ypi = (int)yp;
                float dy = yp - ypi;

                const uchar *sample_row = src->data + ypi * src->row_stride;

                for (int x = 0; x < dest->width; ++x) {
                        float xp = x * inv_scale;
                        int xpi = (int)xp;
                        float dx = xp - xpi;

                        const uchar *sample = sample_row + xpi;

                        float Ix0 = sample[0];
                        float Ix1 = sample[1];

                        float Iy0 = sample[src->row_stride];
                        float Iy1 = sample[src->row_stride + 1];

                        short interp = (Ix0*(1.0f - dx) + Ix1*dx) * (1.0f - dy)
                                + (Iy0*(1.0f - dx) + Iy1*dx) * dy;

                        if (interp < 0)
                                interp = 0;
                        else if (interp > 255)
                                interp = 255;

                        row[x] = interp;
                }
        }
}

void nx_image_downsample(struct NXImage *dest, const struct NXImage *src, NXBool antialias, uchar *filter_buffer)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE("nx_image_downsample", src);
        NX_IMAGE_ASSERT_GRAYSCALE("nx_image_downsample", dest);

        NX_ASSERT_CUSTOM("nx_image_downsample", "NOT IMPLEMENTED", NX_FALSE);

}

void nx_image_smooth_si(struct NXImage *dest, const struct NXImage *src, float sigma, uchar *filter_buffer)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE("nx_image_smooth_si", src);
        NX_IMAGE_ASSERT_GRAYSCALE("nx_image_smooth_si", dest);

        NX_ASSERT_CUSTOM("nx_image_smooth_si", "NOT IMPLEMENTED", NX_FALSE);
}

void nx_image_smooth_s(struct NXImage *dest, const struct NXImage *src, float sigma, uchar *filter_buffer)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE("nx_image_smooth_s", src);
        NX_IMAGE_ASSERT_GRAYSCALE("nx_image_smooth_s", dest);

        NX_ASSERT_CUSTOM("nx_image_smooth_s", "NOT IMPLEMENTED", NX_FALSE);
}
