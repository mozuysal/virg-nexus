/**
 * @file nx_image_io_png.c
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
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/nx_image_io_png.h"

#include <png.h>

#ifndef png_jmpbuf
#  define png_jmpbuf(png_ptr) ((png_ptr)->jmpbuf)
#endif

#ifndef png_infopp_NULL
#  define png_infopp_NULL (png_infopp)NULL
#endif

#ifndef int_p_NULL
# define int_p_NULL (int*)NULL
#endif

#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_io.h"

static NXResult save_as_png(const struct NXImage *img, FILE *fout)
{
        NX_ASSERT_PTR(img);
        NX_IMAGE_ASSERT_UCHAR(img);
        NX_ASSERT_PTR(img->data.uc);
        NX_ASSERT_PTR(fout);

        png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
        if (!png_ptr) {
                NX_ERROR(NX_LOG_TAG, "Could not create PNG write structure");
                return NX_FAIL;
        }

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
                NX_ERROR(NX_LOG_TAG, "Could not create PNG info pointer");
                png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
                return NX_FAIL;
        }

        if (setjmp(png_jmpbuf(png_ptr))) {
                NX_ERROR(NX_LOG_TAG, "Error creating jump for PNG pointer");
                png_destroy_write_struct(&png_ptr, &info_ptr);
                return NX_FAIL;
        }

        int color_type;
        switch (img->type) {
        case NX_IMAGE_GRAYSCALE: color_type = PNG_COLOR_TYPE_GRAY; break;
        case NX_IMAGE_RGBA: color_type = PNG_COLOR_TYPE_RGBA; break;
        default:
                NX_ERROR(NX_LOG_TAG, "Unsupported image type for saving as PNG!");
                png_destroy_write_struct(&png_ptr, &info_ptr);
                return NX_FAIL;
        }

        /* png_set_compression_level(png_ptr, 1); // Set to lowest compression level */

        png_init_io(png_ptr, fout);
        png_set_IHDR(png_ptr, info_ptr, img->width, img->height, 8,
                     color_type, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
        png_write_info(png_ptr, info_ptr);

        if ((unsigned int)img->height > PNG_UINT_32_MAX/sizeof(png_bytep)) {
                png_error(png_ptr, "Image is too tall to process in memory");
                return NX_FAIL;
        }

        png_bytepp row_pointers = NX_NEW(img->height, png_bytep);
        if (!row_pointers) {
                NX_ERROR(NX_LOG_TAG, "Error creating PNG row pointers");
                png_destroy_write_struct(&png_ptr, &info_ptr);
                return NX_FAIL;
        }

        for (png_int_32 k = 0; k < img->height; k++) {
                row_pointers[k] = (unsigned char*)(img->data.uc + k*img->row_stride);
        }

        png_write_image(png_ptr, row_pointers);
        png_write_end(png_ptr, info_ptr);

        png_destroy_write_struct(&png_ptr, &info_ptr);
        nx_free(row_pointers);

        return NX_OK;
}

static NXResult load_as_png(struct NXImage *img, FILE *fin,
                            enum NXImageLoadMode mode)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(fin);
        NX_IMAGE_ASSERT_UCHAR(img);

        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                     NULL, NULL, NULL);
        if (!png_ptr) {
                NX_ERROR(NX_LOG_TAG, "Could not create PNG read structure");
                return NX_FAIL;
        }

        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
                NX_ERROR(NX_LOG_TAG, "Could not create PNG info pointer");
                png_destroy_read_struct(&png_ptr, png_infopp_NULL,
                                        png_infopp_NULL);
                return NX_FAIL;
        }

        if (setjmp(png_jmpbuf(png_ptr))) {
                NX_ERROR(NX_LOG_TAG, "Could not set jump point for reading PNG file");
                png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
                return NX_FAIL;
        }

        png_init_io(png_ptr, fin);
        png_read_info(png_ptr, info_ptr);

        png_uint_32 stream_width, stream_height;
        int bit_depth, color_type, interlace_type;
        png_get_IHDR(png_ptr, info_ptr, &stream_width, &stream_height, &bit_depth, &color_type,
                     &interlace_type, int_p_NULL, int_p_NULL);

        png_set_strip_16(png_ptr);
        if (color_type == PNG_COLOR_TYPE_GA) {
                png_set_strip_alpha(png_ptr); /*(not recommended). */
        }

        png_set_packing(png_ptr);
        if (color_type == PNG_COLOR_TYPE_PALETTE) {
                png_set_palette_to_rgb(png_ptr);
        }

        png_set_expand(png_ptr);

        enum NXImageType output_type = NX_IMAGE_RGBA;
        if (mode == NX_IMAGE_LOAD_GRAYSCALE) {
                output_type = NX_IMAGE_GRAYSCALE;
                png_set_rgb_to_gray_fixed(png_ptr, 1, 30000, 59000);
                png_set_strip_alpha(png_ptr); /*(not recommended). */
        } else if (mode == NX_IMAGE_LOAD_RGBA) {
                output_type = NX_IMAGE_RGBA;

                if (color_type == PNG_COLOR_TYPE_GRAY ||
                    color_type == PNG_COLOR_TYPE_GA) {
                        png_set_gray_to_rgb(png_ptr);
                        png_set_add_alpha(png_ptr, 255, PNG_FILLER_AFTER);
                }
        } else {
                if (color_type == PNG_COLOR_TYPE_GRAY ||
                    color_type == PNG_COLOR_TYPE_GA) {
                        output_type = NX_IMAGE_GRAYSCALE;
                }

                if(color_type == PNG_COLOR_TYPE_RGB ||
                   color_type == PNG_COLOR_TYPE_PALETTE) {
                        png_set_add_alpha(png_ptr, 255, PNG_FILLER_AFTER);
                }
        }

        if (stream_width  != (unsigned int)img->width ||
            stream_height != (unsigned int)img->height ||
            output_type   != img->type) {
                nx_image_resize(img, stream_width, stream_height,
                                NX_IMAGE_STRIDE_DEFAULT, output_type,
                                NX_IMAGE_UCHAR);
        }

        png_bytepp row_pointers = NX_NEW(img->height, png_bytep);
        for (int row = 0; row < img->height; row++) {
                row_pointers[row] = img->data.uc + row * img->row_stride;
        }

        png_read_image(png_ptr, row_pointers);
        png_read_end(png_ptr, info_ptr);

        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);

        nx_free(row_pointers);

        return NX_OK;
}

void nx_image_xsave_png(const struct NXImage *img, const char *filename)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(filename);
        NX_ASSERT_CUSTOM("Only UCHAR images can be saved directly as PNG files", img->dtype==NX_IMAGE_UCHAR);

        FILE *png_stream = nx_xfopen(filename, "wb");

        if (save_as_png(img, png_stream) != NX_OK) {
                NX_FATAL(NX_LOG_TAG, "Can not save image as PNG to %s", filename);
        }

        nx_xfclose(png_stream, filename);
}

void nx_image_xload_png(struct NXImage *img, const char *filename, enum NXImageLoadMode mode)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(filename);
        NX_ASSERT_CUSTOM("Only UCHAR images can be loaded directly from PNG files", img->dtype==NX_IMAGE_UCHAR);

        FILE *png_stream = nx_xfopen(filename, "rb");
        if (load_as_png(img, png_stream, mode) != NX_OK)
                NX_FATAL(NX_LOG_TAG, "Error loading PNG stream from file %s!", filename);
        nx_xfclose(png_stream, filename);
}

NXResult nx_image_save_png(const struct NXImage *img, const char *filename)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(filename);
        NX_ASSERT_CUSTOM("Only UCHAR images can be saved directly as PNG files", img->dtype==NX_IMAGE_UCHAR);

        FILE *png_stream = nx_fopen(filename, "wb");
        if (!png_stream) {
                return NX_FAIL;
        }

        if (save_as_png(img, png_stream) != NX_OK) {
                NX_ERROR(NX_LOG_TAG, "Can not save image as PNG to %s", filename);
                fclose(png_stream);
                return NX_FAIL;
        }

        NXResult res = nx_fclose(png_stream, filename);
        return res;
}

NXResult nx_image_load_png(struct NXImage *img, const char *filename, enum NXImageLoadMode mode)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(filename);
        NX_ASSERT_CUSTOM("Only UCHAR images can be loaded directly from PNG files", img->dtype==NX_IMAGE_UCHAR);

        FILE *png_stream = nx_fopen(filename, "rb");
        if (!png_stream) {
                return NX_FAIL;
        }

        NXResult res_read = load_as_png(img, png_stream, mode);
        if (res_read != NX_OK)
                NX_ERROR(NX_LOG_TAG, "Error loading PNG stream from file %s!", filename);
        NXResult res_close = nx_fclose(png_stream, filename);

        if (res_read == NX_OK)
                return res_close;
        else
                return res_read;
}
