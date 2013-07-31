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

void nx_image_wrap(struct NXImage *img, uchar *data, int width, int height, int row_stride, enum NXImageType type, NXBool own_memory)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(data);
        NX_ASSERT(row_stride >= width);

        size_t sz = row_stride * height * sizeof(uchar);
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
                nx_filter_copy_to_buffer_uc(dest->width, buffer, dest_col, dest->row_stride, nky / 2, NX_BORDER_MIRROR);
                nx_convolve_sym_s_uc(dest->width, buffer, nk, kernel);
                for (int y = 0; y < dest->height; ++y)
                        dest_col[y*dest->row_stride] = buffer[y];
        }

        nx_free(kernel);

        // Clean-up if allocated buffer
        if (!filter_buffer)
                nx_free(buffer);
}

static NXResult save_as_pnm(const struct NXImage *img, FILE *pnm)
{
       if (img->type == NX_IMAGE_GRAYSCALE) {
                const char *magic_head = "P5";
                fprintf(pnm, "%s\n%d %d %d\n", magic_head, img->width, img->height, 255);

                for (int y = 0; y < img->height; ++y) {
                        const uchar *row = img->data + y * img->row_stride;
                        fwrite((const void*)row, sizeof(uchar), img->width, pnm);
                }
        } else if (img->type == NX_IMAGE_RGBA) {
                const char *magic_head = "P6";
                fprintf(pnm, "%s\n%d %d\n%d\n", magic_head, img->width, img->height, 255);

                for (int y = 0; y < img->height; ++y) {
                        const uchar *row = img->data + y*img->row_stride;
                        for (int x = 0; x < img->width; ++x) {
                                fwrite((const void*)(row+4*x), sizeof(uchar), 3, pnm);
                        }
                }
        } else {
                return NX_FAIL;
        }

       return NX_OK;
}

void nx_image_xsave_pnm(const struct NXImage *img, const char *filename)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(filename);

        FILE *pnm = nx_xfopen(filename, "wb");

        if (save_as_pnm(img, pnm) != NX_OK) {
                nx_fatal(NX_LOG_TAG, "Can not save image of unknown type '%d' as PNM", (int)img->type);
        }

        nx_xfclose(pnm, filename);
}

NXResult nx_image_save_pnm(const struct NXImage *img, const char *filename)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(filename);

        FILE *pnm = nx_fopen(filename, "wb");
        if (!pnm) {
                return NX_FAIL;
        }

        if (save_as_pnm(img, pnm) != NX_OK) {
                nx_error(NX_LOG_TAG, "Can not save image of unknown type '%d' as PNM", (int)img->type);
                fclose(pnm);
                return NX_FAIL;
        }

        NXResult res = nx_fclose(pnm, filename);

        return res;
}

static void read_pnm_data(struct NXImage *img, enum NXImageType img_type,
                          FILE *pnm, int pnm_width, int pnm_height, enum NXImageType pnm_type)
{
        int n_ch = nx_image_n_channels(img_type);
        nx_image_resize(img, pnm_width, pnm_height, pnm_width*n_ch, img_type);

        if (pnm_type == NX_IMAGE_GRAYSCALE) {
                if (img_type == NX_IMAGE_GRAYSCALE) {
                        for (int y = 0; y < img->height; ++y) {
                                uchar* row = img->data + y * img->row_stride;
                                fread((void*)row, sizeof(uchar), img->width, pnm);
                        }
                } else {
                        for (int y = 0; y < img->height; ++y) {
                                uchar* row = img->data + y * img->row_stride;
                                for(int x = 0; x < img->width; ++x) {
                                        uchar value;
                                        fread((void*)&value, sizeof(uchar), 1, pnm);
                                        row[4*x] = value;
                                        row[4*x+1] = value;
                                        row[4*x+2] = value;
                                        row[4*x+3] = 255;
                                }
                        }
                }
        } else {
                if (img_type == NX_IMAGE_GRAYSCALE) {
                        for (int y = 0; y < img->height; ++y) {
                                uchar* row = img->data + y * img->row_stride;
                                for (int x = 0; x < img->width; ++x) {
                                        uchar value[3];
                                        fread((void*)&value, sizeof(uchar), 3, pnm);
                                        row[x] = nx_rgb_to_gray(value[0], value[1], value[2]);
                                }
                        }
                } else {
                        for (int y = 0; y < img->height; ++y) {
                                uchar* row = img->data + y * img->row_stride;
                                for (int x = 0; x < img->width; ++x) {
                                        fread((void*)(row + 4*x), sizeof(uchar), 3, pnm);
                                        row[4*x+3] = 255;
                                }
                        }
                }
        }
}

void nx_image_xload_pnm(struct NXImage *img, const char *filename,
                            enum NXImageLoadMode mode)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(filename);

        FILE *pnm = nx_xfopen(filename, "rb");

        char ch1, ch2;
        if (fscanf(pnm, "%c%c", &ch1, &ch2) != 2) {
                nx_io_fatal_exit(NX_LOG_TAG, "Could not read image header from %s", filename);
        }

        if(ch1 != 'P' || (ch2 != '5' && ch2 != '6')) {
                nx_fatal(NX_LOG_TAG, "Image %s is not a PNM file", filename);
        }

        enum NXImageType pnm_type;
        if(ch2 == '5') {
                pnm_type = NX_IMAGE_GRAYSCALE;
        } else if(ch2 == '6') {
                pnm_type = NX_IMAGE_RGBA;
        }

        int pnm_width;
        int pnm_height;
        int pnm_levels;

        if (fscanf(pnm, "%d %d %d", &pnm_width, &pnm_height, &pnm_levels) != 3) {
                nx_io_fatal_exit(NX_LOG_TAG, "Could not read image attributes from %s", filename);
        }

        // Skip a new line
        char buffer[256];
        fgets(buffer, sizeof(buffer), pnm);

        enum NXImageType img_type;
        switch (mode) {
        case NX_IMAGE_LOAD_GRAYSCALE:
                img_type = NX_IMAGE_GRAYSCALE;
                break;
        case NX_IMAGE_LOAD_RGBA:
                img_type = NX_IMAGE_RGBA;
                break;
        case NX_IMAGE_LOAD_AS_IS:
        default:
                img_type = pnm_type;
                break;
        }

        read_pnm_data(img, img_type, pnm, pnm_width, pnm_height, pnm_type);

        nx_xfclose(pnm, filename);
}

NXResult nx_image_load_pnm(struct NXImage *img, const char *filename,
                           enum NXImageLoadMode mode)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(filename);

        FILE *pnm = nx_fopen(filename, "rb");
        if (!pnm) {
                return NX_FAIL;
        }

        char ch1, ch2;
        if (fscanf(pnm, "%c%c", &ch1, &ch2) != 2) {
                nx_io_error(NX_LOG_TAG, "Could not read image header from %s", filename);
                fclose(pnm);
                return NX_FAIL;
        }

        if(ch1 != 'P' || (ch2 != '5' && ch2 != '6')) {
                nx_error(NX_LOG_TAG, "Image %s is not a PNM file", filename);
                fclose(pnm);
                return NX_FAIL;
        }

        enum NXImageType pnm_type;
        if(ch2 == '5') {
                pnm_type = NX_IMAGE_GRAYSCALE;
        } else if(ch2 == '6') {
                pnm_type = NX_IMAGE_RGBA;
        }

        int pnm_width;
        int pnm_height;
        int pnm_levels;

        if (fscanf(pnm, "%d %d %d", &pnm_width, &pnm_height, &pnm_levels) != 3) {
                nx_io_error(NX_LOG_TAG, "Could not read image attributes from %s", filename);
                fclose(pnm);
                return NX_FAIL;
        }

        // Skip a new line
        char buffer[256];
        fgets(buffer, sizeof(buffer), pnm);

        enum NXImageType img_type;
        switch (mode) {
        case NX_IMAGE_LOAD_GRAYSCALE:
                img_type = NX_IMAGE_GRAYSCALE;
                break;
        case NX_IMAGE_LOAD_RGBA:
                img_type = NX_IMAGE_RGBA;
                break;
        case NX_IMAGE_LOAD_AS_IS:
        default:
                img_type = pnm_type;
                break;
        }

        read_pnm_data(img, img_type, pnm, pnm_width, pnm_height, pnm_type);

        NXResult res = nx_fclose(pnm, filename);
        return res;
}
