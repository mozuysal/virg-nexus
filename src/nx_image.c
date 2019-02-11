/**
 * @file nx_image.c
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
#include "virg/nexus/nx_image.h"

#include <stdlib.h>
#include <string.h>

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_io.h"
#include "virg/nexus/nx_math.h"
#include "virg/nexus/nx_mat234.h"
#include "virg/nexus/nx_mem_block.h"
#include "virg/nexus/nx_colorspace.h"
#include "virg/nexus/nx_filter.h"
#include "virg/nexus/nx_transform_2d.h"
#include "virg/nexus/nx_image_warp.h"

static const double NX_IMAGE_SMOOTH_KERNEL_LOSS = 0.003;

struct NXImage *nx_image_alloc()
{
        struct NXImage *img = NX_NEW(1, struct NXImage);

        img->width = 0;
        img->height = 0;
        img->type = NX_IMAGE_GRAYSCALE;
        img->dtype = NX_IMAGE_UCHAR;
        img->n_channels = 1;

        img->mem = nx_mem_block_alloc();
        img->data.v = NULL;
        img->row_stride = 0;

        return img;
}

struct NXImage *nx_image_new(int width, int height,
                             enum NXImageType type, enum NXImageDataType dtype)
{
        NX_ASSERT(width >= 0);
        NX_ASSERT(height >= 0);

        struct NXImage *img = nx_image_alloc();
        nx_image_resize(img, width, height, NX_IMAGE_STRIDE_DEFAULT, type, dtype);

        return img;
}

struct NXImage *nx_image_new_gray(int width, int height,
                                  enum NXImageDataType dtype)
{
        return nx_image_new(width, height, NX_IMAGE_GRAYSCALE, dtype);
}

struct NXImage *nx_image_new_rgba(int width, int height,
                                  enum NXImageDataType dtype)
{
        return nx_image_new(width, height, NX_IMAGE_RGBA, dtype);
}

struct NXImage *nx_image_new_gray_uc(int width, int height)
{
        return nx_image_new(width, height, NX_IMAGE_GRAYSCALE, NX_IMAGE_UCHAR);
}

struct NXImage *nx_image_new_rgba_uc(int width, int height)
{
        return nx_image_new(width, height, NX_IMAGE_RGBA, NX_IMAGE_UCHAR);
}

struct NXImage *nx_image_new_gray_f32(int width, int height)
{
        return nx_image_new(width, height, NX_IMAGE_GRAYSCALE, NX_IMAGE_FLOAT32);
}

struct NXImage *nx_image_new_rgba_f32(int width, int height)
{
        return nx_image_new(width, height, NX_IMAGE_RGBA, NX_IMAGE_FLOAT32);
}

struct NXImage *nx_image_new_like(const struct NXImage* src)
{
        NX_ASSERT_PTR(src);

        return nx_image_new(src->width, src->height, src->type, src->dtype);
}

void nx_image_free(struct NXImage *img)
{
        if (img) {
                nx_mem_block_free(img->mem);
                nx_free(img);
        }
}

void nx_image_resize(struct NXImage *img, int width, int height, int row_stride,
                     enum NXImageType type, enum NXImageDataType dtype)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT(width >= 0);
        NX_ASSERT(height >= 0);

        if (img->width == width &&
            img->height == height &&
            img->row_stride == row_stride &&
            img->type == type &&
            img->dtype == dtype)
                return;

        int n_ch = nx_image_n_channels(type);
        int rs = n_ch * width;
        if (row_stride < rs)
                row_stride = rs;

        size_t length = row_stride * height;
        size_t n_bytes = nx_image_bytes_per_channel(dtype);
        nx_mem_block_resize(img->mem, length*n_bytes);

        img->width = width;
        img->height = height;
        img->type = type;
        img->n_channels = n_ch;
        img->dtype = dtype;
        switch (dtype) {
        case NX_IMAGE_UCHAR: img->data.uc = (uchar *)img->mem->ptr; break;
        case NX_IMAGE_FLOAT32: img->data.f32 = (float *)img->mem->ptr; break;
        default: NX_FATAL(NX_LOG_TAG, "Unhandled switch case for data type!");
        }
        img->row_stride = row_stride;
}

void nx_image_resize_like(struct NXImage *img, const struct NXImage *src)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(src);

        nx_image_resize(img, src->width, src->height,
                        NX_IMAGE_STRIDE_DEFAULT, src->type, src->dtype);
}

void nx_image_release(struct NXImage *img)
{
        NX_ASSERT_PTR(img);

        nx_mem_block_release(img->mem);

        img->width = 0;
        img->height = 0;
        img->type = NX_IMAGE_GRAYSCALE;
        img->n_channels = 1;
        img->data.v = NULL;
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
        dest->data.v = dest->mem->ptr;
        dest->row_stride = src->row_stride;
}

static inline void convert_image_gray_to_rgba(struct NXImage* dest,
                                              const struct NXImage* src)
{
        NX_ASSERT(src->dtype == dest->dtype);

        switch (src->dtype) {
        case NX_IMAGE_UCHAR:
                nx_convert_gray_to_rgba_uc(src->width, src->height,
                                           dest->data.uc, dest->row_stride,
                                           src->data.uc, src->row_stride);
                break;
        case NX_IMAGE_FLOAT32:
                nx_convert_gray_to_rgba_f32(src->width, src->height,
                                            dest->data.f32, dest->row_stride,
                                            src->data.f32, src->row_stride);
                break;
        default:
                NX_FATAL(NX_LOG_TAG, "Unhandled switch case for image data type");
        }
}

static inline void convert_image_rgba_to_gray(struct NXImage* dest,
                                              const struct NXImage* src)
{
        NX_ASSERT(src->dtype == dest->dtype);

        switch (src->dtype) {
        case NX_IMAGE_UCHAR:
                nx_convert_rgba_to_gray_uc(src->width, src->height,
                                           dest->data.uc, dest->row_stride,
                                           src->data.uc, src->row_stride);
                break;
        case NX_IMAGE_FLOAT32:
                nx_convert_rgba_to_gray_f32(src->width, src->height,
                                            dest->data.f32, dest->row_stride,
                                            src->data.f32, src->row_stride);
                break;
        default:
                NX_FATAL(NX_LOG_TAG, "Unhandled switch case for image data type");
        }
}

void nx_image_convert_type(struct NXImage* img, enum NXImageType type)
{
        NX_ASSERT_PTR(img);

        if (img->type == type || img->data.v == NULL)
                return;

        struct NXImage* cpy = nx_image_copy0(img);
        nx_image_resize(img, img->width, img->height, NX_IMAGE_STRIDE_DEFAULT,
                        type, img->dtype);
        if (cpy->type == NX_IMAGE_GRAYSCALE) {
                if (img->type == NX_IMAGE_RGBA) {
                        convert_image_gray_to_rgba(img, cpy);
                } else {
                        NX_FATAL(NX_LOG_TAG, "Unhandled target image type");
                }
        } else if (cpy->type == NX_IMAGE_RGBA) {
                if (img->type == NX_IMAGE_GRAYSCALE) {
                        convert_image_rgba_to_gray(img, cpy);
                } else {
                        NX_FATAL(NX_LOG_TAG, "Unhandled target image type");
                }
        } else {
                NX_FATAL(NX_LOG_TAG, "Unhandled source image type");
        }

        nx_image_free(cpy);
}


void nx_image_set_zero(struct NXImage *img)
{
        NX_ASSERT_PTR(img);

        nx_mem_block_set_zero(img->mem);
}

void nx_image_wrap(struct NXImage *img, void *data, int width, int height,
                   int row_stride, enum NXImageType type,
                   enum NXImageDataType dtype, NXBool own_memory)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(data);
        NX_ASSERT(row_stride >= width);

        size_t n_bytes = nx_image_bytes_per_channel(dtype);
        size_t sz = row_stride * height * n_bytes;
        nx_mem_block_wrap(img->mem, data, sz, sz, own_memory);

        img->width = width;
        img->height = height;
        img->type = type;
        img->dtype = dtype;
        img->n_channels = nx_image_n_channels(type);
        img->data.v = img->mem->ptr;
        img->row_stride = row_stride;
}

#define NX_IMAGE_SWAP(f,T) do { T tmp = img0->f; img0->f = img1->f; img1->f = tmp; } while(0)
void nx_image_swap(struct NXImage *img0, struct NXImage *img1)
{
        NX_ASSERT_PTR(img0);
        NX_ASSERT_PTR(img1);

        nx_mem_block_swap(img0->mem, img1->mem);
        img0->data.v = img0->mem->ptr;
        img1->data.v = img1->mem->ptr;

        NX_IMAGE_SWAP(width,int);
        NX_IMAGE_SWAP(height,int);
        NX_IMAGE_SWAP(row_stride,int);
        NX_IMAGE_SWAP(n_channels,int);
        NX_IMAGE_SWAP(type,enum NXImageType);
        NX_IMAGE_SWAP(dtype,enum NXImageDataType);
}
#undef NX_IMAGE_SWAP

void nx_image_scale_uc(struct NXImage *dest, const struct NXImage *src, float scale_f)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE_UCHAR(src);

        int dest_width = src->width * scale_f;
        int dest_height = src->height * scale_f;
        int n_ch = nx_image_n_channels(src->type);
        int dest_row_stride = dest_width*n_ch;
        nx_image_resize(dest, dest_width, dest_height, dest_row_stride, src->type, src->dtype);

        float inv_scale = 1.0f / scale_f;
        for (int y = 0; y < dest->height; ++y) {
                uchar *row = dest->data.uc + y * dest->row_stride;

                float yp = y * inv_scale;
                int ypi = (int)yp;
                float dy = yp - ypi;

                const uchar *sample_row = src->data.uc + ypi * src->row_stride;

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

void nx_image_scale_f32(struct NXImage *dest, const struct NXImage *src, float scale_f)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE(src);
        NX_ASSERT_CUSTOM("Image data must be of type float", src->dtype==NX_IMAGE_FLOAT32);

        int dest_width = src->width * scale_f;
        int dest_height = src->height * scale_f;
        int n_ch = nx_image_n_channels(src->type);
        int dest_row_stride = dest_width*n_ch;
        nx_image_resize(dest, dest_width, dest_height, dest_row_stride, src->type, src->dtype);

        float inv_scale = 1.0f / scale_f;
        for (int y = 0; y < dest->height; ++y) {
                float *row = dest->data.f32 + y * dest->row_stride;

                float yp = y * inv_scale;
                int ypi = (int)yp;
                float dy = yp - ypi;

                const float *sample_row = src->data.f32 + ypi * src->row_stride;

                for (int x = 0; x < dest->width; ++x) {
                        float xp = x * inv_scale;
                        int xpi = (int)xp;
                        float dx = xp - xpi;

                        const float *sample = sample_row + xpi;

                        float Ix0 = sample[0];
                        float Ix1 = sample[1];

                        float Iy0 = sample[src->row_stride];
                        float Iy1 = sample[src->row_stride + 1];

                        float interp = (Ix0*(1.0f - dx) + Ix1*dx) * (1.0f - dy)
                                + (Iy0*(1.0f - dx) + Iy1*dx) * dy;

                        row[x] = interp;
                }
        }
}

void nx_image_scale(struct NXImage *dest, const struct NXImage *src, float scale_f)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE(src);

        switch (src->dtype) {
        case NX_IMAGE_UCHAR: nx_image_scale_uc(dest, src, scale_f); break;
        case NX_IMAGE_FLOAT32: nx_image_scale_f32(dest, src, scale_f); break;
        default: NX_FATAL(NX_LOG_TAG, "Unhandled switch case for image data type");
        }
}

#define DEFINE_DOWNSAMPLE_FUNC(F,T)                                     \
        void nx_image_downsample_##F(struct NXImage *dest,              \
                                     const struct NXImage *src)         \
        {                                                               \
                NX_ASSERT_PTR(src);                                     \
                NX_ASSERT_PTR(dest);                                    \
                                                                        \
                int dest_width = src->width / 2;                        \
                int dest_height = src->height / 2;                      \
                int n_ch = nx_image_n_channels(src->type);              \
                int dest_row_stride = dest_width*n_ch;                  \
                nx_image_resize(dest, dest_width, dest_height, dest_row_stride, \
                                src->type, src->dtype);                 \
                                                                        \
                for (int y = 0; y < dest->height; ++y) {                \
                        const T *src_row = src->data.F + 2*y * src->row_stride; \
                        T *dest_row = dest->data.F + y * dest->row_stride; \
                        for (int x = 0; x < dest->width; ++x) {         \
                                dest_row[x] = src_row[2*x];             \
                        }                                               \
                }                                                       \
        }

DEFINE_DOWNSAMPLE_FUNC(uc,uchar)
DEFINE_DOWNSAMPLE_FUNC(f32,float)
#undef DEFINE_DOWNSAMPLE_FUNC
void nx_image_downsample(struct NXImage *dest, const struct NXImage *src)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE(src);

        switch (src->dtype) {
        case NX_IMAGE_UCHAR: nx_image_downsample_uc(dest, src); break;
        case NX_IMAGE_FLOAT32: nx_image_downsample_f32(dest, src); break;
        default: NX_FATAL(NX_LOG_TAG, "Unhandled switch case for image data type");
        }
}

#define DEFINE_DOWNSAMPLE_AA_X_FUNC(F,T)                                \
        void nx_image_downsample_aa_x_##F(struct NXImage *dest, const struct NXImage *src) \
        {                                                               \
                NX_ASSERT_PTR(src);                                     \
                NX_ASSERT_PTR(dest);                                    \
                NX_IMAGE_ASSERT_GRAYSCALE(src);                         \
                                                                        \
                int dest_width = src->width / 2;                        \
                int dest_height = src->height;                          \
                nx_image_resize(dest, dest_width, dest_height, 0, src->type, src->dtype); \
                                                                        \
                const float norm_f = 1.0f / (1 + 6 + 11 + 6 + 1);       \
                for (int y = 0; y < dest->height; ++y) {                \
                        const T *src_row = src->data.F + y * src->row_stride; \
                        T *dest_row = dest->data.F + y * dest->row_stride; \
                        dest_row[0] = (2*src_row[2] + 12*src_row[1]     \
                                       + 11 * src_row[0]) * norm_f;     \
                        for (int x = 1; x < dest->width-1; ++x) {       \
                                float sum = src_row[2*x-2] + src_row[2*x+2] \
                                        + 6 * (src_row[2*x-1] + src_row[2*x+1]) \
                                        + 11 * src_row[2*x];            \
                                dest_row[x] = sum * norm_f;             \
                        }                                               \
                        int twodw = 2*dest->width;                      \
                        if (twodw == src->width)                        \
                                dest_row[dest->width-1] = (src_row[twodw-4] \
                                                           + 6 * (src_row[twodw-3] \
                                                                  + src_row[twodw-1]) \
                                                           + 12 * src_row[twodw-2]) * norm_f; \
                        else                                            \
                                dest_row[dest->width-1] = (src_row[twodw-4] \
                                                           + src_row[twodw] \
                                                           + 6 * (src_row[twodw-3] \
                                                                  + src_row[twodw-1]) \
                                                           + 11 * src_row[twodw-2]) * norm_f; \
                }                                                       \
        }

DEFINE_DOWNSAMPLE_AA_X_FUNC(uc,uchar)
DEFINE_DOWNSAMPLE_AA_X_FUNC(f32,float)
#undef DEFINE_DOWNSAMPLE_AA_X_FUNC
void nx_image_downsample_aa_x(struct NXImage *dest, const struct NXImage *src)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE(src);

        switch (src->dtype) {
        case NX_IMAGE_UCHAR: nx_image_downsample_aa_x_uc(dest, src); break;
        case NX_IMAGE_FLOAT32: nx_image_downsample_aa_x_f32(dest, src); break;
        default: NX_FATAL(NX_LOG_TAG, "Unhandled switch case for image data type");
        }
}

#define DEFINE_DOWNSAMPLE_AA_Y_FUNC(F,T)                                \
        void nx_image_downsample_aa_y_##F(struct NXImage *dest, const struct NXImage *src) \
        {                                                               \
                NX_ASSERT_PTR(src);                                     \
                NX_ASSERT_PTR(dest);                                    \
                NX_IMAGE_ASSERT_GRAYSCALE(src);                         \
                                                                        \
                int dest_width = src->width;                            \
                int dest_height = src->height / 2;                      \
                nx_image_resize(dest, dest_width, dest_height, 0, src->type, src->dtype); \
                                                                        \
                const float norm_f = 1.0f / (1 + 6 + 11 + 6 + 1);       \
                const int ss = src->row_stride;                         \
                const int ds = dest->row_stride;                        \
                for (int x = 0; x < dest->width; ++x) {                 \
                        const T *src_col = src->data.F + x;        \
                        T *dest_col = dest->data.F + x;            \
                        dest_col[0] = (2*src_col[2*ss] + 12 * src_col[ss] \
                                       + 11 * src_col[0]) * norm_f;     \
                        for (int y = 1; y < dest->height-1; ++y) {      \
                                int yds = y*dest->row_stride;           \
                                float sum = src_col[(2*y-2)*ss] + src_col[(2*y+2)*ss] \
                                        + 6 * (src_col[(2*y-1)*ss] + src_col[(2*y+1)*ss]) \
                                        + 11 * src_col[2*y*ss];         \
                                dest_col[yds] = sum * norm_f;           \
                        }                                               \
                        const int dlast_col = (dest->height-1)*ds;      \
                        int twodh = 2*dest->height;                     \
                        if (twodh == src->height)                       \
                                dest_col[dlast_col] = (src_col[(twodh-4)*ss] \
                                                       + 6 * (src_col[(twodh-3)*ss] \
                                                              + src_col[(twodh-1)*ss]) \
                                                       + 12 * src_col[(twodh-2)*ss]) * norm_f; \
                        else                                            \
                                dest_col[dlast_col] = (src_col[(twodh-4)*ss] \
                                                       + src_col[(twodh)*ss] \
                                                       + 6 * (src_col[(twodh-3)*ss] \
                                                              + src_col[(twodh-1)*ss]) \
                                                       + 11 * src_col[(twodh-2)*ss]) * norm_f; \
                                                                        \
                }                                                       \
        }

DEFINE_DOWNSAMPLE_AA_Y_FUNC(uc,uchar)
DEFINE_DOWNSAMPLE_AA_Y_FUNC(f32,float)
#undef DEFINE_DOWNSAMPLE_AA_Y_FUNC
void nx_image_downsample_aa_y(struct NXImage *dest, const struct NXImage *src)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE(src);

        switch (src->dtype) {
        case NX_IMAGE_UCHAR: nx_image_downsample_aa_y_uc(dest, src); break;
        case NX_IMAGE_FLOAT32: nx_image_downsample_aa_y_f32(dest, src); break;
        default: NX_FATAL(NX_LOG_TAG, "Unhandled switch case for image data type");
        }
}

float *nx_image_filter_buffer_alloc(int width, int height,
                                    float sigma_x, float sigma_y)
{
        int nkx = nx_kernel_size_min_gaussian(sigma_x, NX_IMAGE_SMOOTH_KERNEL_LOSS);
        int nky = nx_kernel_size_min_gaussian(sigma_y, NX_IMAGE_SMOOTH_KERNEL_LOSS);
        int nk_max = nx_max_i(nkx, nky);

        int max_dim = nx_max_i(width, height);

        return nx_filter_buffer_alloc(max_dim, nk_max / 2);
}

void nx_image_smooth(struct NXImage *dest, const struct NXImage *src,
                     float sigma_x, float sigma_y, float *filter_buffer)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE(src);

        int dest_width = src->width;
        int dest_height = src->height;
        int n_ch = nx_image_n_channels(src->type);
        int dest_row_stride = dest_width*n_ch;
        nx_image_resize(dest, dest_width, dest_height, dest_row_stride, src->type, src->dtype);

        float *buffer = filter_buffer;
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
        nx_kernel_sym_gaussian(nk, kernel, sigma_x);
        for (int y = 0; y < src->height; ++y) {
                switch (src->dtype) {
                case NX_IMAGE_UCHAR:
                        nx_filter_copy_to_buffer1_uc(src->width, buffer,
                                                     src->data.uc + y * src->row_stride,
                                                     nkx / 2, NX_BORDER_MIRROR);
                        break;
                case NX_IMAGE_FLOAT32:
                        nx_filter_copy_to_buffer1(src->width, buffer,
                                                  src->data.f32 + y * src->row_stride,
                                                  nkx / 2, NX_BORDER_MIRROR);
                        break;
                default:
                        NX_FATAL(NX_LOG_TAG, "Unhandled switch case for image data type");
                }
                nx_convolve_sym(src->width, buffer, nk, kernel);
                switch (src->dtype) {
                case NX_IMAGE_UCHAR:
                        for (int x = 0; x < dest->width; ++x)
                                dest->data.uc[y*dest->row_stride+x] = (uchar)buffer[x];
                        break;
                case NX_IMAGE_FLOAT32:
                        memcpy(dest->data.f32 + y*dest->row_stride, buffer,
                               dest->width*dest->n_channels*sizeof(float));
                        break;
                default:
                        NX_FATAL(NX_LOG_TAG, "Unhandled switch case for image data type");
                }
        }

        // Smooth in y-direction
        nk = nky / 2 + 1;
        nx_kernel_sym_gaussian(nk, kernel, sigma_y);
        for (int x = 0; x < dest->width; ++x) {
                switch (src->dtype) {
                case NX_IMAGE_UCHAR:
                        nx_filter_copy_to_buffer_uc(dest->height, buffer,
                                                    dest->data.uc + x,
                                                    dest->row_stride, nky / 2,
                                                    NX_BORDER_MIRROR);
                        break;
                case NX_IMAGE_FLOAT32:
                        nx_filter_copy_to_buffer(dest->height, buffer,
                                                 dest->data.f32 + x,
                                                 dest->row_stride, nky / 2,
                                                 NX_BORDER_MIRROR);
                        break;
                default:
                        NX_FATAL(NX_LOG_TAG, "Unhandled switch case for image data type");
                }

                nx_convolve_sym(dest->height, buffer, nk, kernel);

                switch (src->dtype) {
                case NX_IMAGE_UCHAR:
                        for (int y = 0; y < dest->height; ++y)
                                dest->data.uc[x + y*dest->row_stride] = (uchar)buffer[y];
                        break;
                case NX_IMAGE_FLOAT32:
                        for (int y = 0; y < dest->height; ++y)
                                dest->data.f32[x + y*dest->row_stride] = buffer[y];
                        break;
                default:
                        NX_FATAL(NX_LOG_TAG, "Unhandled switch case for image data type");
                }
        }

        nx_free(kernel);

        // Clean-up if allocated buffer
        if (!filter_buffer)
                nx_free(buffer);
}

#define DEFINE_DERIV_X_FUNC(F,T,S)                                       \
        void nx_image_deriv_x_##F(struct NXImage *dest,                 \
                                  const struct NXImage *src)            \
        {                                                               \
                NX_ASSERT_PTR(src);                                     \
                NX_ASSERT_PTR(dest);                                    \
                NX_IMAGE_ASSERT_GRAYSCALE(src);                         \
                                                                        \
                nx_image_resize(dest, src->width, src->height, NX_IMAGE_STRIDE_DEFAULT, \
                                src->type, NX_IMAGE_FLOAT32);           \
                nx_image_set_zero(dest);                                \
                                                                        \
                for (int y = 1; y < src->height-1; ++y) {                 \
                        const T *src_row = src->data.F + y*src->row_stride; \
                        float *dest_row = dest->data.f32 + y*dest->row_stride; \
                        for (int x = 1; x < dest->width-1; ++x) {       \
                                dest_row[x] = (src_row[x+1]-src_row[x-1])/S; \
                        }                                               \
                }                                                       \
        }

DEFINE_DERIV_X_FUNC(uc,uchar,255.0f)
DEFINE_DERIV_X_FUNC(f32,float,1)
#undef DEFINE_DERIV_X_FUNC

void nx_image_deriv_x(struct NXImage *dest, const struct NXImage *src)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE(src);

        switch (src->dtype) {
        case NX_IMAGE_UCHAR: nx_image_deriv_x_uc(dest, src); break;
        case NX_IMAGE_FLOAT32: nx_image_deriv_x_f32(dest, src); break;
        default: NX_FATAL(NX_LOG_TAG, "Unhandled switch case for image data type");
        }
}

#define DEFINE_DERIV_Y_FUNC(F,T,S)                                       \
        void nx_image_deriv_y_##F(struct NXImage *dest,                 \
                                  const struct NXImage *src)            \
        {                                                               \
                NX_ASSERT_PTR(src);                                     \
                NX_ASSERT_PTR(dest);                                    \
                NX_IMAGE_ASSERT_GRAYSCALE(src);                         \
                                                                        \
                nx_image_resize(dest, src->width, src->height, NX_IMAGE_STRIDE_DEFAULT, \
                                src->type, NX_IMAGE_FLOAT32);           \
                nx_image_set_zero(dest);                                \
                                                                        \
                for (int y = 1; y < src->height-1; ++y) {                 \
                        const T *src_row_m = src->data.F + (y-1)*src->row_stride; \
                        const T *src_row_p = src->data.F + (y+1)*src->row_stride; \
                        float *dest_row = dest->data.f32 + y*dest->row_stride; \
                        for (int x = 1; x < dest->width-1; ++x) {       \
                                dest_row[x] = (src_row_p[x]-src_row_m[x])/S; \
                        }                                               \
                }                                                       \
        }

DEFINE_DERIV_Y_FUNC(uc,uchar,255.0f)
DEFINE_DERIV_Y_FUNC(f32,float,1)
#undef DEFINE_DERIV_Y_FUNC

void nx_image_deriv_y(struct NXImage *dest, const struct NXImage *src)
{
        NX_ASSERT_PTR(src);
        NX_ASSERT_PTR(dest);
        NX_IMAGE_ASSERT_GRAYSCALE(src);

        switch (src->dtype) {
        case NX_IMAGE_UCHAR: nx_image_deriv_y_uc(dest, src); break;
        case NX_IMAGE_FLOAT32: nx_image_deriv_y_f32(dest, src); break;
        default: NX_FATAL(NX_LOG_TAG, "Unhandled switch case for image data type");
        }
}
