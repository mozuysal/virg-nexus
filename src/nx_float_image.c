/**
 * @file nx_float_image.c
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
#include "virg/nexus/nx_float_image.h"

#include <stdlib.h>
#include <string.h>

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_io.h"
#include "virg/nexus/nx_math.h"
#include "virg/nexus/nx_mem_block.h"
#include "virg/nexus/nx_filter.h"
#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_spline.h"
#include "virg/nexus/nx_transform_2d.h"

static const double NX_FLOAT_IMAGE_SMOOTH_KERNEL_LOSS = 0.003;

struct NXFloatImage *nx_float_image_alloc()
{
        struct NXFloatImage *fimg = NX_NEW(1, struct NXFloatImage);

        fimg->width = 0;
        fimg->height = 0;
        fimg->n_channels = 1;

        fimg->mem = nx_mem_block_alloc();
        fimg->data = NULL;
        fimg->row_stride = 0;

        return fimg;
}

struct NXFloatImage *nx_float_image_new(int width, int height, int n_channels)
{
        struct NXFloatImage *fimg = nx_float_image_alloc();
        nx_float_image_resize(fimg, width, height, n_channels, width*n_channels);

        return fimg;
}

struct NXFloatImage *nx_float_image_new_gray(int width, int height)
{
        return nx_float_image_new(width, height, 1);
}

struct NXFloatImage *nx_float_image_new_rgba(int width, int height)
{
        return nx_float_image_new(width, height, 4);
}

void nx_float_image_free(struct NXFloatImage *fimg)
{
        if (fimg) {
                nx_mem_block_free(fimg->mem);
                nx_free(fimg);
        }
}

void nx_float_image_resize(struct NXFloatImage *fimg, int width, int height, int n_channels, int row_stride)
{
        NX_ASSERT_PTR(fimg);
        NX_ASSERT(width > 0);
        NX_ASSERT(height > 0);
        NX_ASSERT(n_channels > 0);

        if (fimg->width == width &&
            fimg->height == height &&
            fimg->row_stride == row_stride &&
            fimg->n_channels == n_channels)
                return;

        int rs = n_channels * width;
        if (row_stride < rs)
                row_stride = rs;

        size_t length = row_stride * height;
        nx_mem_block_resize(fimg->mem, length * sizeof(float));

        fimg->width = width;
        fimg->height = height;
        fimg->n_channels = n_channels;
        fimg->data = (float *)fimg->mem->ptr;
        fimg->row_stride = row_stride;
}

void nx_float_image_release(struct NXFloatImage *fimg)
{
        NX_ASSERT_PTR(fimg);

        nx_mem_block_release(fimg->mem);

        fimg->width = 0;
        fimg->height = 0;
        fimg->n_channels = 1;
        fimg->data = NULL;
        fimg->row_stride = 0;
}

struct NXFloatImage *nx_float_image_copy0(const struct NXFloatImage *fimg)
{
        NX_ASSERT_PTR(fimg);

        struct NXFloatImage *cpy = nx_float_image_alloc();
        nx_float_image_copy(cpy, fimg);
        return cpy;
}

void nx_float_image_copy(struct NXFloatImage *dest, const struct NXFloatImage *src)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);

        nx_mem_block_copy(dest->mem, src->mem);

        dest->width = src->width;
        dest->height = src->height;
        dest->n_channels = src->n_channels;
        dest->data = (float *)dest->mem->ptr;
        dest->row_stride = src->row_stride;
}

void nx_float_image_set_zero(struct NXFloatImage *fimg)
{
        NX_ASSERT_PTR(fimg);

        nx_mem_block_set_zero(fimg->mem);
}

void nx_float_image_wrap(struct NXFloatImage *fimg, float *data, int width, int height, int n_channels, int row_stride, NXBool own_memory)
{
        NX_ASSERT_PTR(fimg);
        NX_ASSERT_PTR(data);
        NX_ASSERT(row_stride >= width*n_channels);

        size_t sz = row_stride * height * sizeof(float);
        nx_mem_block_wrap(fimg->mem, (uchar *)data, sz, own_memory);

        fimg->width = width;
        fimg->height = height;
        fimg->n_channels = n_channels;
        fimg->data = (float *)fimg->mem->ptr;
        fimg->row_stride = row_stride;
}

void nx_float_image_swap(struct NXFloatImage *fimg0, struct NXFloatImage *fimg1)
{
        NX_ASSERT_PTR(fimg0);
        NX_ASSERT_PTR(fimg1);

        nx_mem_block_swap(fimg0->mem, fimg1->mem);
        fimg0->data = (float *)fimg0->mem->ptr;
        fimg1->data = (float *)fimg1->mem->ptr;

        int tmpi;
        tmpi = fimg0->width; fimg1->width = fimg0->width; fimg1->width = tmpi;
        tmpi = fimg0->height; fimg1->height = fimg0->height; fimg1->height = tmpi;
        tmpi = fimg0->row_stride; fimg1->row_stride = fimg0->row_stride; fimg1->row_stride = tmpi;
        tmpi = fimg0->n_channels; fimg1->n_channels = fimg0->n_channels; fimg1->n_channels = tmpi;
}

float *nx_float_image_filter_buffer_alloc(int width, int height, float sigma_x, float sigma_y)
{
        int nkx = nx_kernel_size_min_gaussian(sigma_x, NX_FLOAT_IMAGE_SMOOTH_KERNEL_LOSS);
        int nky = nx_kernel_size_min_gaussian(sigma_y, NX_FLOAT_IMAGE_SMOOTH_KERNEL_LOSS);
        int nk_max = nx_max_i(nkx, nky);

        int max_dim = nx_max_i(width, height);

        return nx_filter_buffer_alloc_s(max_dim, nk_max / 2);
}

void nx_float_image_smooth_s(struct NXFloatImage *dest, const struct NXFloatImage *src,
                             float sigma_x, float sigma_y, float *filter_buffer)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_FLOAT_IMAGE_ASSERT_GRAYSCALE(src);
        NX_FLOAT_IMAGE_ASSERT_GRAYSCALE(dest);

        nx_float_image_resize(dest, src->width, src->height, src->n_channels, 0);

        float *buffer = filter_buffer;
        if (!buffer) {
                buffer = nx_float_image_filter_buffer_alloc(src->width, src->height,
                                                            sigma_x, sigma_y);
        }

        int nkx = nx_kernel_size_min_gaussian(sigma_x, NX_FLOAT_IMAGE_SMOOTH_KERNEL_LOSS);
        int nky = nx_kernel_size_min_gaussian(sigma_y, NX_FLOAT_IMAGE_SMOOTH_KERNEL_LOSS);
        int nk_max = nx_max_i(nkx, nky);
        int nk_sym = nk_max / 2 + 1;
        float *kernel = NX_NEW_S(nk_sym);

        // Smooth in x-direction
        int nk = nkx / 2 + 1;
        nx_kernel_sym_gaussian_s(nk, kernel, sigma_x);
        for (int y = 0; y < src->height; ++y) {
                const float *src_row = src->data + y * src->row_stride;
                float *dest_row = dest->data + y * dest->row_stride;
                nx_filter_copy_to_buffer1_s(src->width, buffer, src_row, nkx / 2, NX_BORDER_MIRROR);
                nx_convolve_sym_s(src->width, buffer, nk, kernel);
                memcpy(dest_row, buffer, dest->width * dest->n_channels * sizeof(float));
        }

        // Smooth in y-direction
        nk = nky / 2 + 1;
        nx_kernel_sym_gaussian_s(nk, kernel, sigma_y);
        for (int x = 0; x < dest->width; ++x) {
                float *dest_col = dest->data + x;
                nx_filter_copy_to_buffer_s(dest->width, buffer, dest_col, dest->row_stride, nky / 2, NX_BORDER_MIRROR);
                nx_convolve_sym_s(dest->width, buffer, nk, kernel);
                for (int y = 0; y < dest->height; ++y)
                        dest_col[y*dest->row_stride] = buffer[y];
        }

        nx_free(kernel);

        // Clean-up if allocated buffer
        if (!filter_buffer)
                nx_free(buffer);
}

static NXResult save_as_raw(const struct NXFloatImage *fimg, FILE *raw)
{
        if (1 != fwrite((const void*)(&fimg->width), sizeof(fimg->width), 1, raw)) return NX_FAIL;
        if (1 != fwrite((const void*)(&fimg->height), sizeof(fimg->height), 1, raw)) return NX_FAIL;
        if (1 != fwrite((const void*)(&fimg->n_channels), sizeof(fimg->n_channels), 1, raw)) return NX_FAIL;
        size_t n_items = fimg->width*fimg->n_channels;
        for (int y = 0; y < fimg->height; ++y) {
                const float *row = fimg->data + y * fimg->row_stride;
                if (n_items != fwrite((const void*)row, sizeof(float), n_items, raw)) return NX_FAIL;
        }

       return NX_OK;
}

void nx_float_image_xsave_raw(const struct NXFloatImage *fimg, const char *filename)
{
        NX_ASSERT_PTR(fimg);
        NX_ASSERT_PTR(filename);

        FILE *raw = nx_xfopen(filename, "wb");

        if (save_as_raw(fimg, raw) != NX_OK) {
                nx_fatal(NX_LOG_TAG, "Could not save image %s as RAW", filename);
        }

        nx_xfclose(raw, filename);
}

NXResult nx_float_image_save_raw(const struct NXFloatImage *fimg, const char *filename)
{
        NX_ASSERT_PTR(fimg);
        NX_ASSERT_PTR(filename);

        FILE *raw = nx_fopen(filename, "wb");
        if (!raw) {
                return NX_FAIL;
        }

        if (save_as_raw(fimg, raw) != NX_OK) {
                nx_error(NX_LOG_TAG, "Could not save image %s as RAW", filename);
                fclose(raw);
                return NX_FAIL;
        }

        NXResult res = nx_fclose(raw, filename);

        return res;
}

static NXResult read_raw_data(struct NXFloatImage *fimg, FILE *raw)
{
        int raw_width;
        int raw_height;
        int raw_n_channels;

        if (1 != fread((void*)(&raw_width), sizeof(fimg->width), 1, raw)) return NX_FAIL;
        if (1 != fread((void*)(&raw_height), sizeof(fimg->height), 1, raw)) return NX_FAIL;
        if (1 != fread((void*)(&raw_n_channels), sizeof(fimg->n_channels), 1, raw)) return NX_FAIL;

        nx_float_image_resize(fimg, raw_width, raw_height, raw_n_channels, 0);

        size_t n_items = fimg->width * fimg->n_channels;
        for (int y = 0; y < fimg->height; ++y) {
                float* row = fimg->data + y*fimg->row_stride;
                if (n_items != fread((void*)row, sizeof(float), n_items, raw))
                        return NX_FAIL;
        }

        return NX_OK;
}

void nx_float_image_xload_raw(struct NXFloatImage *fimg, const char *filename)
{
        NX_ASSERT_PTR(fimg);
        NX_ASSERT_PTR(filename);

        FILE *raw = nx_xfopen(filename, "rb");

        if (read_raw_data(fimg, raw) != NX_OK) {
                nx_io_fatal_exit(NX_LOG_TAG, "Could not read RAW image from %s", filename);
        }

        nx_xfclose(raw, filename);
}

NXResult nx_float_image_load_raw(struct NXFloatImage *fimg, const char *filename)
{
        NX_ASSERT_PTR(fimg);
        NX_ASSERT_PTR(filename);

        FILE *raw = nx_fopen(filename, "rb");
        if (!raw) {
                return NX_FAIL;
        }

        if (read_raw_data(fimg, raw) != NX_OK) {
                nx_io_error(NX_LOG_TAG, "Could not read RAW image from %s", filename);
                fclose(raw);
                return NX_FAIL;
        }

        NXResult res = nx_fclose(raw, filename);
        return res;
}

struct NXFloatImage *nx_float_image_from_uchar0(const struct NXImage *img)
{
        NX_ASSERT_PTR(img);

        struct NXFloatImage *fimg = nx_float_image_alloc();
        nx_float_image_from_uchar(fimg, img);

        return fimg;
}

void nx_float_image_from_uchar(struct NXFloatImage *fimg, const struct NXImage *img)
{
        NX_ASSERT_PTR(fimg);
        NX_ASSERT_PTR(img);

        nx_float_image_resize(fimg, img->width, img->height, img->n_channels, 0);

        float norm_f = 1.0f / 255.0f;
        int n_row_elems = img->width * img->n_channels;
        for (int y = 0; y < img->height; ++y) {
                const uchar *row = img->data + y*img->row_stride;
                float *frow = fimg->data + y*fimg->row_stride;
                for (int x = 0; x < n_row_elems; ++x) {
                        frow[x] = row[x] * norm_f;
                }
        }
}

void nx_float_image_to_uchar(struct NXImage *img, const struct NXFloatImage *fimg)
{
        NX_ASSERT_PTR(fimg);
        NX_ASSERT_PTR(img);

        enum NXImageType type;
        switch (fimg->n_channels) {
        case 1:
                type = NX_IMAGE_GRAYSCALE;
                break;
        case 4:
                type = NX_IMAGE_RGBA;
                break;
        default:
                nx_fatal(NX_LOG_TAG, "Can not convert float image with %d channels to uchar!",
                         fimg->n_channels);
        }

        nx_image_resize(img, fimg->width, fimg->height, 0, type);

        int n_row_elems = img->width * img->n_channels;
        for (int y = 0; y < img->height; ++y) {
                const float *frow = fimg->data + y*fimg->row_stride;
                uchar *row = img->data + y*img->row_stride;
                for (int x = 0; x < n_row_elems; ++x) {
                        int rx = frow[x] * 255.0f;
                        if (rx < 0) rx = 0;
                        else if (rx > 255) rx = 255;
                        row[x] = rx;
                }
        }
}

void nx_float_image_spline_coeff_of(struct NXFloatImage *fimg, const struct NXImage *img)
{
        NX_ASSERT_PTR(fimg);
        NX_ASSERT_PTR(img);
        NX_IMAGE_ASSERT_GRAYSCALE(img);

        nx_float_image_resize(fimg, img->width, img->height, 1, 0);

        nx_spline_from_grayscale(img->width, img->height,
                                 fimg->data, fimg->row_stride,
                                 img->data, img->row_stride);

}

void nx_float_image_transform_affine(struct NXImage *dest, const struct NXFloatImage *src_coeff,
                                     const float *t_src2dest, enum NXImageWarpBackgroundMode bg_mode)
{
        NX_ASSERT_PTR(src_coeff);
        NX_ASSERT_PTR(dest);
        NX_FLOAT_IMAGE_ASSERT_GRAYSCALE(src_coeff);
        NX_IMAGE_ASSERT_GRAYSCALE(dest);

        float t_dest2src[9];
        nx_mat3_inv_sd(t_dest2src, t_src2dest);

        float nrm_f = 1.0f / t_dest2src[8];
        for (int i = 0; i < 9; ++i)
                t_dest2src[i] *= nrm_f;

        if (dest->data == NULL)
                nx_image_resize(dest, src_coeff->width, src_coeff->height, 0, NX_IMAGE_GRAYSCALE);

        nx_image_warp_affine_spline(dest->width, dest->height, dest->data, dest->row_stride,
                                    src_coeff->width, src_coeff->height, src_coeff->data, src_coeff->row_stride,
                                    t_dest2src, bg_mode);
}

void nx_float_image_transform_affine_prm(struct NXImage *dest, const struct NXFloatImage *src_coeff,
                                         float lambda, float psi, float theta, float phi,
                                         enum NXImageWarpBackgroundMode bg_mode)
{
        NX_ASSERT_PTR(src_coeff);
        NX_ASSERT_PTR(dest);
        NX_FLOAT_IMAGE_ASSERT_GRAYSCALE(src_coeff);
        NX_IMAGE_ASSERT_GRAYSCALE(dest);

        if (dest->data == NULL)
                nx_image_resize(dest, src_coeff->width, src_coeff->height, 0, NX_IMAGE_GRAYSCALE);

        float t_src2dest[9];
        nx_transform_2d_fill_affine_s(t_src2dest, dest->width / 2, dest->height / 2, lambda,
                                      psi, theta, phi, -src_coeff->width / 2, -src_coeff->height / 2);

        nx_float_image_transform_affine(dest, src_coeff, t_src2dest, bg_mode);
}
