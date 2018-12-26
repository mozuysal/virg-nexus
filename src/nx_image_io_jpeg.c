/**
 * @file nx_image_io_jpeg.c
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2015 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/nx_image_io_jpeg.h"

#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>

#include <jpeglib.h>

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_io.h"
#include "virg/nexus/nx_colorspace.h"

typedef struct _my_error_mgr {
        struct jpeg_error_mgr pub;
        jmp_buf setjmp_buffer;
} my_error_mgr;

typedef my_error_mgr* my_error_ptr;

METHODDEF(void) my_error_exit (j_common_ptr cinfo)
{
        my_error_ptr myerr = (my_error_ptr) cinfo->err;
        (*cinfo->err->output_message) (cinfo);
        longjmp(myerr->setjmp_buffer, 1);
}

static NXResult save_as_jpeg(const struct NXImage *img, FILE *jpeg_stream)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(jpeg_stream);
        NX_ASSERT_CUSTOM("Only UCHAR images can be saved directly as JPEG files", img->dtype==NX_IMAGE_UCHAR);

        struct jpeg_compress_struct cinfo;
        my_error_mgr jerr;
        int quality = 100;

        cinfo.err = jpeg_std_error(&jerr.pub);
        jerr.pub.error_exit = my_error_exit;

        if (setjmp(jerr.setjmp_buffer)) {
                jpeg_destroy_compress(&cinfo);
                return NX_FAIL;
        }

        jpeg_create_compress(&cinfo);
        jpeg_stdio_dest(&cinfo, jpeg_stream);

        cinfo.image_width  = img->width;
        cinfo.image_height = img->height;
        cinfo.input_components = ((img->n_channels == 1) ? 1 : 3);
        cinfo.num_components = ((img->n_channels) == 1 ? 1 : 3);
        cinfo.in_color_space = ((img->n_channels == 1) ? JCS_GRAYSCALE : JCS_RGB);

        jpeg_set_defaults(&cinfo);
        jpeg_set_quality(&cinfo, quality, TRUE);
        jpeg_start_compress(&cinfo, TRUE);

        if (img->type == NX_IMAGE_GRAYSCALE) {
                while (cinfo.next_scanline < cinfo.image_height) {
                        JSAMPROW row_pointer = (JSAMPROW)((unsigned char*)(img->data.uc + cinfo.next_scanline * img->row_stride));
                        jpeg_write_scanlines(&cinfo, &row_pointer, 1);
                }
        } else if (img->type == NX_IMAGE_RGBA) {
                int x;
                uchar* buffer = NX_NEW_UC(3*img->width);
                JSAMPROW row_pointer = (JSAMPROW)(buffer);
                while (cinfo.next_scanline < cinfo.image_height) {
                        const unsigned char* read_row = img->data.uc + cinfo.next_scanline * img->row_stride;
                        for(x = 0; x < img->width; ++x) {
                                buffer[3*x]   = read_row[4*x];
                                buffer[3*x+1] = read_row[4*x+1];
                                buffer[3*x+2] = read_row[4*x+2];
                        }
                        jpeg_write_scanlines(&cinfo, &row_pointer, 1);
                }
                nx_free(buffer);
        } else {
                return NX_FAIL;
        }

        jpeg_finish_compress(&cinfo);
        jpeg_destroy_compress(&cinfo);

        return NX_OK;
}

void nx_image_xsave_jpeg(const struct NXImage *img, const char *filename)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(filename);
        NX_ASSERT_CUSTOM("Only UCHAR images can be saved directly as JPEG files", img->dtype==NX_IMAGE_UCHAR);

        FILE *jpeg_stream = nx_xfopen(filename, "wb");

        if (save_as_jpeg(img, jpeg_stream) != NX_OK) {
                NX_FATAL(NX_LOG_TAG, "Can not save image as JPEG to %s", filename);
        }

        nx_xfclose(jpeg_stream, filename);
}

NXResult nx_image_save_jpeg(const struct NXImage *img, const char *filename)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(filename);
        NX_ASSERT_CUSTOM("Only UCHAR images can be saved directly as JPEG files", img->dtype==NX_IMAGE_UCHAR);

        FILE *jpeg_stream = nx_fopen(filename, "wb");
        if (!jpeg_stream) {
                return NX_FAIL;
        }

        if (save_as_jpeg(img, jpeg_stream) != NX_OK) {
                NX_ERROR(NX_LOG_TAG, "Can not save image as JPEG to %s", filename);
                fclose(jpeg_stream);
                return NX_FAIL;
        }

        NXResult res = nx_fclose(jpeg_stream, filename);

        return res;
}

static NXResult load_as_jpeg(struct NXImage *img, FILE *jpeg_stream,
                             enum NXImageLoadMode mode)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(jpeg_stream);
        NX_ASSERT_CUSTOM("Only UCHAR images can be loaded directly from JPEG files", img->dtype==NX_IMAGE_UCHAR);

        struct jpeg_decompress_struct cinfo;
        my_error_mgr jerr;

        cinfo.err = jpeg_std_error(&jerr.pub);
        jerr.pub.error_exit = my_error_exit;

        if (setjmp(jerr.setjmp_buffer)) {
                jpeg_destroy_decompress(&cinfo);
                return NX_FAIL;
        }

        jpeg_create_decompress(&cinfo);
        jpeg_stdio_src(&cinfo, jpeg_stream);
        jpeg_read_header(&cinfo, TRUE);

        if (mode == NX_IMAGE_LOAD_GRAYSCALE) {
                cinfo.output_components = 1;
                cinfo.out_color_components = 1;
                cinfo.out_color_space = JCS_GRAYSCALE;
        } else if (mode == NX_IMAGE_LOAD_RGBA) {
                cinfo.output_components = 3;
                cinfo.out_color_components = 3;
                cinfo.out_color_space = JCS_RGB;
        }

        jpeg_start_decompress(&cinfo);
        nx_image_resize(img, cinfo.output_width, cinfo.output_height,
                        NX_IMAGE_STRIDE_DEFAULT, cinfo.output_components == 1 ? NX_IMAGE_GRAYSCALE : NX_IMAGE_RGBA, img->dtype);

        if (cinfo.out_color_space == JCS_GRAYSCALE) {
                while (cinfo.output_scanline < cinfo.output_height) {
                        JSAMPROW row_pointer = (JSAMPROW)(img->data.uc + cinfo.output_scanline * img->row_stride);
                        jpeg_read_scanlines(&cinfo, &row_pointer, 1);
                }
        } else {
                int x;
                uchar* buffer = NX_NEW_UC(3*img->width);
                JSAMPROW row_pointer = (JSAMPROW)(buffer);
                while (cinfo.output_scanline < cinfo.output_height) {
                        uchar* write_row = img->data.uc + cinfo.output_scanline * img->row_stride;
                        jpeg_read_scanlines(&cinfo, &row_pointer, 1);
                        for(x = 0; x < img->width; ++x) {
                                write_row[4*x]     = buffer[3*x];
                                write_row[4*x + 1] = buffer[3*x + 1];
                                write_row[4*x + 2] = buffer[3*x + 2];
                                write_row[4*x + 3] = 255;
                        }
                }
                nx_free(buffer);
        }

        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);

        return NX_OK;
}

void nx_image_xload_jpeg(struct NXImage *img, const char *filename,
                         enum NXImageLoadMode mode)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(filename);
        NX_ASSERT_CUSTOM("Only UCHAR images can be loaded directly from JPEG files", img->dtype==NX_IMAGE_UCHAR);

        FILE *jpeg_stream = nx_xfopen(filename, "rb");
        if (load_as_jpeg(img, jpeg_stream, mode) != NX_OK)
                NX_FATAL(NX_LOG_TAG, "Error loading JPEG stream from file %s!", filename);
        nx_xfclose(jpeg_stream, filename);
}

NXResult nx_image_load_jpeg(struct NXImage *img, const char *filename,
                            enum NXImageLoadMode mode)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(filename);
        NX_ASSERT_CUSTOM("Only UCHAR images can be loaded directly from JPEG files", img->dtype==NX_IMAGE_UCHAR);

        FILE *jpeg_stream = nx_fopen(filename, "rb");
        if (!jpeg_stream) {
                return NX_FAIL;
        }

        NXResult res_read;
        if ((res_read = load_as_jpeg(img, jpeg_stream, mode)) != NX_OK)
                NX_ERROR(NX_LOG_TAG, "Error loading JPEG stream from file %s!", filename);
        NXResult res_close = nx_fclose(jpeg_stream, filename);

        if (res_read == NX_OK)
                return res_close;
        else
                return res_read;
}
