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
#include <string.h>

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_io.h"
#include "virg/nexus/nx_math.h"
#include "virg/nexus/nx_mem_block.h"
#include "virg/nexus/nx_colorspace.h"
#include "virg/nexus/nx_filter.h"
#include "virg/nexus/nx_transform_2d.h"
#include "virg/nexus/nx_image_warp.h"

static const double NX_IMAGE_SMOOTH_KERNEL_LOSS = 0.003;

static void nx_process_aa_buffer(uchar *buffer, int n);

struct NXImage *nx_image_alloc()
{
        struct NXImage *img = NX_NEW(1, struct NXImage);

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
        int n_ch = nx_image_n_channels(type);
        nx_image_resize(img, width, height, width*n_ch, type);

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

void nx_image_resize(struct NXImage *img, int width, int height, int row_stride, enum NXImageType type)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT(width > 0);
        NX_ASSERT(height > 0);

        if (img->width == width &&
            img->height == height &&
            img->row_stride == row_stride &&
            img->type == type)
                return;

        int n_ch = nx_image_n_channels(type);
        int rs = n_ch * width;
        if (row_stride < rs)
                row_stride = rs;

        size_t length = row_stride * height;
        nx_mem_block_resize(img->mem, length * sizeof(uchar));

        img->width = width;
        img->height = height;
        img->type = type;
        img->n_channels = n_ch;
        img->data = img->mem->ptr;
        img->row_stride = row_stride;
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

void nx_image_sub_rect_of(struct NXImage *sub_rect, struct NXImage *img, int tl_x, int tl_y, int rect_width, int rect_height)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(sub_rect);
        NX_ASSERT(tl_x > 0);
        NX_ASSERT(tl_y > 0);
        NX_ASSERT(tl_x + rect_width <= img->width);
        NX_ASSERT(tl_y + rect_height <= img->height);

        nx_image_wrap(sub_rect, img->data + tl_y*img->row_stride + tl_x*img->n_channels,
                      rect_width, rect_height, img->row_stride, img->type, NX_FALSE);
}

void nx_image_wrap(struct NXImage *img, uchar *data, int width, int height, int row_stride, enum NXImageType type, NXBool own_memory)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(data);
        NX_ASSERT(row_stride >= width);

        size_t sz = row_stride * height * sizeof(uchar);
        nx_mem_block_wrap(img->mem, data, sz, sz, own_memory);

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

        int n_ch = nx_image_n_channels(dest->type);
        int dest_row_stride = src->width*n_ch;
        nx_image_resize(dest, src->width, src->height, dest_row_stride, dest->type);

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
                nx_fatal(NX_LOG_TAG, "Unknown destination image type %d!", (int)dest->type);
                return;
        }
}

void nx_image_scale(struct NXImage *dest, const struct NXImage *src, float scale_f)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE(src);
        NX_IMAGE_ASSERT_GRAYSCALE(dest);

        int dest_width = src->width * scale_f;
        int dest_height = src->height * scale_f;
        int n_ch = nx_image_n_channels(src->type);
        int dest_row_stride = dest_width*n_ch;
        nx_image_resize(dest, dest_width, dest_height, dest_row_stride, src->type);

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

void nx_image_downsample(struct NXImage *dest, const struct NXImage *src)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE(src);
        NX_IMAGE_ASSERT_GRAYSCALE(dest);

        int dest_width = src->width / 2;
        int dest_height = src->height / 2;
        int n_ch = nx_image_n_channels(src->type);
        int dest_row_stride = dest_width*n_ch;
        nx_image_resize(dest, dest_width, dest_height, dest_row_stride, src->type);

        for (int y = 0; y < dest->height; ++y) {
                const uchar *src_row = src->data + 2*y * src->row_stride;
                uchar *dest_row = dest->data + y * dest->row_stride;
                for (int x = 0; x < dest->width; ++x) {
                        dest_row[x] = src_row[2*x];
                }
        }
}

void nx_process_aa_buffer(uchar *buffer, int n)
{
        float norm_f = 1.0f / (1 + 6 + 11 + 6 + 1);

        for (int i = 0; i < n; i += 2) {
                int sum = buffer[i] + buffer[i+4] + 6*(buffer[i+1] + buffer[i+3]) + 11 * buffer[i+2];
                buffer[i/2] = sum * norm_f;
        }
}

void nx_image_downsample_aa_x(struct NXImage *dest, const struct NXImage *src)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE(src);
        NX_IMAGE_ASSERT_GRAYSCALE(dest);

        int dest_width = src->width / 2;
        int dest_height = src->height;
        nx_image_resize(dest, dest_width, dest_height, 0, src->type);

        uchar *aa_buff = nx_filter_buffer_alloc(src->width, 2);
        for (int y = 0; y < dest->height; ++y) {
                const uchar *src_row = src->data + y * src->row_stride;
                nx_filter_copy_to_buffer1_uc(src->width, aa_buff, src_row, 2, NX_BORDER_MIRROR);
                nx_process_aa_buffer(aa_buff, src->width);

                uchar *dest_row = dest->data + y * dest->row_stride;
                for (int x = 0; x < dest->width; ++x)
                        dest_row[x] = aa_buff[x];
        }
        nx_free(aa_buff);
}

void nx_image_downsample_aa_y(struct NXImage *dest, const struct NXImage *src)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE(src);
        NX_IMAGE_ASSERT_GRAYSCALE(dest);

        int dest_width = src->width;
        int dest_height = src->height / 2;
        nx_image_resize(dest, dest_width, dest_height, 0, src->type);

        uchar *aa_buff = nx_filter_buffer_alloc(src->height, 2);
        for (int x = 0; x < dest->width; ++x) {
                const uchar *src_col = src->data + x;
                nx_filter_copy_to_buffer_uc(src->height, aa_buff, src_col, src->row_stride, 2, NX_BORDER_MIRROR);
                nx_process_aa_buffer(aa_buff, src->height);

                uchar *dest_col = dest->data + x;
                for (int y = 0; y < dest->height; ++y)
                        dest_col[y * dest->row_stride] = aa_buff[y];
        }
        nx_free(aa_buff);
}

uchar *nx_image_filter_buffer_alloc(int width, int height, float sigma_x, float sigma_y)
{
        int nkx = nx_kernel_size_min_gaussian(sigma_x, NX_IMAGE_SMOOTH_KERNEL_LOSS);
        int nky = nx_kernel_size_min_gaussian(sigma_y, NX_IMAGE_SMOOTH_KERNEL_LOSS);
        int nk_max = nx_max_i(nkx, nky);

        int max_dim = nx_max_i(width, height);

        return nx_filter_buffer_alloc(max_dim, nk_max / 2);
}


void nx_image_smooth_si(struct NXImage *dest, const struct NXImage *src,
                        float sigma_x, float sigma_y, uchar *filter_buffer)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE(src);
        NX_IMAGE_ASSERT_GRAYSCALE(dest);

        int dest_width = src->width;
        int dest_height = src->height;
        int n_ch = nx_image_n_channels(src->type);
        int dest_row_stride = dest_width*n_ch;
        nx_image_resize(dest, dest_width, dest_height, dest_row_stride, src->type);

        uchar *buffer = filter_buffer;
        if (!buffer) {
                buffer = nx_image_filter_buffer_alloc(src->width, src->height,
                                                      sigma_x, sigma_y);
        }

        int nkx = nx_kernel_size_min_gaussian(sigma_x, NX_IMAGE_SMOOTH_KERNEL_LOSS);
        int nky = nx_kernel_size_min_gaussian(sigma_y, NX_IMAGE_SMOOTH_KERNEL_LOSS);
        int nk_max = nx_max_i(nkx, nky);
        int nk_sym = nk_max / 2 + 1;
        short *kernel = NX_NEW_SI(nk_sym);

        // Smooth in x-direction
        int nk = nkx / 2 + 1;
        short k_sum = nx_kernel_sym_gaussian_si(nk, kernel, sigma_x);
        for (int y = 0; y < src->height; ++y) {
                const uchar *src_row = src->data + y * src->row_stride;
                uchar *dest_row = dest->data + y * dest->row_stride;
                nx_filter_copy_to_buffer1_uc(src->width, buffer, src_row, nkx / 2, NX_BORDER_MIRROR);
                nx_convolve_sym_si_uc(src->width, buffer, nk, kernel, k_sum);
                memcpy(dest_row, buffer, dest->width * dest->n_channels * sizeof(uchar));
        }

        // Smooth in y-direction
        nk = nky / 2 + 1;
        k_sum = nx_kernel_sym_gaussian_si(nk, kernel, sigma_y);
        for (int x = 0; x < dest->width; ++x) {
                uchar *dest_col = dest->data + x;
                nx_filter_copy_to_buffer_uc(dest->width, buffer, dest_col, dest->row_stride, nky / 2, NX_BORDER_MIRROR);
                nx_convolve_sym_si_uc(dest->width, buffer, nk, kernel, k_sum);
                for (int y = 0; y < dest->height; ++y)
                        dest_col[y*dest->row_stride] = buffer[y];
        }

        nx_free(kernel);

        // Clean-up if allocated buffer
        if (!filter_buffer)
                nx_free(buffer);
}

void nx_image_smooth_s(struct NXImage *dest, const struct NXImage *src,
                       float sigma_x, float sigma_y, uchar *filter_buffer)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE(src);
        NX_IMAGE_ASSERT_GRAYSCALE(dest);


        int dest_width = src->width;
        int dest_height = src->height;
        int n_ch = nx_image_n_channels(src->type);
        int dest_row_stride = dest_width*n_ch;
        nx_image_resize(dest, dest_width, dest_height, dest_row_stride, src->type);

        uchar *buffer = filter_buffer;
        if (!buffer) {
                buffer = nx_image_filter_buffer_alloc(src->width, src->height,
                                                      sigma_x, sigma_y);
        }

        int nkx = nx_kernel_size_min_gaussian(sigma_x, NX_IMAGE_SMOOTH_KERNEL_LOSS);
        int nky = nx_kernel_size_min_gaussian(sigma_y, NX_IMAGE_SMOOTH_KERNEL_LOSS);
        int nk_max = nx_max_i(nkx, nky);
        int nk_sym = nk_max / 2 + 1;
        float *kernel = NX_NEW_S(nk_sym);

        // Smooth in x-direction
        int nk = nkx / 2 + 1;
        nx_kernel_sym_gaussian_s(nk, kernel, sigma_x);
        for (int y = 0; y < src->height; ++y) {
                const uchar *src_row = src->data + y * src->row_stride;
                uchar *dest_row = dest->data + y * dest->row_stride;
                nx_filter_copy_to_buffer1_uc(src->width, buffer, src_row, nkx / 2, NX_BORDER_MIRROR);
                nx_convolve_sym_s_uc(src->width, buffer, nk, kernel);
                memcpy(dest_row, buffer, dest->width * dest->n_channels * sizeof(uchar));
        }

        // Smooth in y-direction
        nk = nky / 2 + 1;
        nx_kernel_sym_gaussian_s(nk, kernel, sigma_y);
        for (int x = 0; x < dest->width; ++x) {
                uchar *dest_col = dest->data + x;
                nx_filter_copy_to_buffer_uc(dest->height, buffer, dest_col, dest->row_stride, nky / 2, NX_BORDER_MIRROR);
                nx_convolve_sym_s_uc(dest->height, buffer, nk, kernel);
                for (int y = 0; y < dest->height; ++y)
                        dest_col[y*dest->row_stride] = buffer[y];
        }

        nx_free(kernel);

        // Clean-up if allocated buffer
        if (!filter_buffer)
                nx_free(buffer);
}

void nx_image_transform_affine(struct NXImage *dest, const struct NXImage *src,
                               const float *t_src2dest, enum NXImageWarpBackgroundMode bg_mode)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE(src);
        NX_IMAGE_ASSERT_GRAYSCALE(dest);


        float t_dest2src[9];
        nx_mat3_inv_sd(t_dest2src, t_src2dest);

        float nrm_f = 1.0f / t_dest2src[8];
        for (int i = 0; i < 9; ++i)
                t_dest2src[i] *= nrm_f;

        if (dest->data == NULL)
                nx_image_resize(dest, src->width, src->height, 0, src->type);


        nx_image_warp_affine_bilinear(dest->width, dest->height, dest->data, dest->row_stride,
                                      src->width, src->height, src->data, src->row_stride,
                                      t_dest2src, bg_mode);
}

void nx_image_transform_affine_prm(struct NXImage *dest, const struct NXImage *src,
                                   float lambda, float psi, float theta, float phi,
                                   enum NXImageWarpBackgroundMode bg_mode)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE(src);
        NX_IMAGE_ASSERT_GRAYSCALE(dest);

        if (dest->data == NULL)
                nx_image_resize(dest, src->width, src->height, 0, src->type);

        float t_src2dest[9];
        nx_transform_2d_fill_affine_s(t_src2dest, dest->width / 2, dest->height / 2, lambda,
                                      psi, theta, phi, -src->width / 2, -src->height / 2);

        nx_image_transform_affine(dest, src, t_src2dest, bg_mode);
}

uchar nx_image_sample_bilinear(const struct NXImage *img, float xs, float ys)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(img->data);

        int xsi = xs;
        int ysi = ys;

        float u = xs-xsi;
        float v = ys-ysi;
        float up = 1.0f - u;
        float vp = 1.0f - v;

        int idx[2] = { xsi, xsi + 1};
        int idy[2] = { ysi, ysi + 1};

        // Mirror out of bound coordinates
        const int LAST_X = img->width - 1;
        const int LAST_Y = img->height - 1;
        if (idx[0] < 0) {
                const int W2 = img->width * 2 - 2;
                if (idx[0] < 0) idx[0] =  -idx[0] - W2 * (-idx[0] / W2);
                if (idx[1] < 0) idx[1] =  -idx[1] - W2 * (-idx[1] / W2);
        } else if (idx[0] >= LAST_X) {
                const int W2 = img->width * 2 - 2;
                if (idx[0] >= img->width) idx[0] = W2 - idx[0] - W2 * (idx[0] / W2);
                if (idx[1] >= img->width) idx[1] = W2 - idx[1] - W2 * (idx[1] / W2);
        }

        if (idy[0] < 0) {
                const int H2 = img->height * 2 - 2;
                if (idy[0] < 0) idy[0] =  -idy[0] - H2 * (-idy[0] / H2);
                if (idy[1] < 0) idy[1] =  -idy[1] - H2 * (-idy[1] / H2);
        } else if (idy[0] >= LAST_Y) {
                const int H2 = img->height * 2 - 2;
                if (idy[0] >= img->height) idy[0] = H2 - idy[0] - H2 * (idy[0] / H2);
                if (idy[1] >= img->height) idy[1] = H2 - idy[1] - H2 * (idy[1] / H2);
        }

        const uchar *p0 = img->data + idy[0]*img->row_stride;
        const uchar *p1 = img->data + idy[1]*img->row_stride;

        short I = vp*(up*p0[idx[0]] + u*p0[idx[1]])
                + v*(up*p1[idx[0]] + u*p1[idx[1]]);

        if (I < 0) I = 0;
        else if (I > 255) I = 255;

        return I;
}
