/**
 * @file nx_image_io_pnm.c
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
#include "virg/nexus/nx_image_io_pnm.h"

#include <stdlib.h>

#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_io.h"
#include "virg/nexus/nx_colorspace.h"

static NXResult save_as_pnm(const struct NXImage *img, FILE *pnm)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_CUSTOM("Only UCHAR images can be saved directly as PNM files", img->dtype==NX_IMAGE_UCHAR);

        if (img->type == NX_IMAGE_GRAYSCALE) {
                const char *magic_head = "P5";
                fprintf(pnm, "%s\n%d %d %d\n", magic_head, img->width, img->height, 255);

                for (int y = 0; y < img->height; ++y) {
                        const uchar *row = img->data.uc + y * img->row_stride;
                        fwrite((const void*)row, sizeof(uchar), img->width, pnm);
                }
        } else if (img->type == NX_IMAGE_RGBA) {
                const char *magic_head = "P6";
                fprintf(pnm, "%s\n%d %d\n%d\n", magic_head, img->width, img->height, 255);

                for (int y = 0; y < img->height; ++y) {
                        const uchar *row = img->data.uc + y*img->row_stride;
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
        NX_ASSERT_CUSTOM("Only UCHAR images can be saved directly as PNM files", img->dtype==NX_IMAGE_UCHAR);

        FILE *pnm = nx_xfopen(filename, "wb");

        if (save_as_pnm(img, pnm) != NX_OK) {
                NX_FATAL(NX_LOG_TAG, "Can not save image of unknown type '%d' as PNM", (int)img->type);
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
                NX_ERROR(NX_LOG_TAG, "Can not save image of unknown type '%d' as PNM", (int)img->type);
                fclose(pnm);
                return NX_FAIL;
        }

        NXResult res = nx_fclose(pnm, filename);

        return res;
}

static NXResult read_pnm_data(struct NXImage *img, enum NXImageType img_type,
                              FILE *pnm, int pnm_width, int pnm_height, enum NXImageType pnm_type)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(pnm);
        NX_ASSERT_CUSTOM("Only UCHAR images can be loaded directly from PNM files", img->dtype==NX_IMAGE_UCHAR);

        nx_image_resize(img, pnm_width, pnm_height, NX_IMAGE_STRIDE_DEFAULT, img_type, img->dtype);

        if (pnm_type == NX_IMAGE_GRAYSCALE) {
                if (img_type == NX_IMAGE_GRAYSCALE) {
                        for (int y = 0; y < img->height; ++y) {
                                uchar* row = img->data.uc + y * img->row_stride;
                                if (fread((void*)row, sizeof(uchar), img->width, pnm) != img->width)
                                        return NX_FAIL;
                        }
                } else if (img_type == NX_IMAGE_RGBA) {
                        for (int y = 0; y < img->height; ++y) {
                                uchar* row = img->data.uc + y * img->row_stride;
                                for(int x = 0; x < img->width; ++x) {
                                        uchar value;
                                        if (fread((void*)&value, sizeof(uchar), 1, pnm) != 1)
                                                return NX_FAIL;
                                        row[4*x] = value;
                                        row[4*x+1] = value;
                                        row[4*x+2] = value;
                                        row[4*x+3] = 255;
                                }
                        }
                } else {
                        NX_FATAL(NX_LOG_TAG, "Unsupported image type while loading grayscale PNM!");
                }
        } else {
                if (img_type == NX_IMAGE_GRAYSCALE) {
                        for (int y = 0; y < img->height; ++y) {
                                uchar* row = img->data.uc + y * img->row_stride;
                                for (int x = 0; x < img->width; ++x) {
                                        uchar value[3];
                                        if (fread((void*)&value, sizeof(uchar), 3, pnm) != 3)
                                                return NX_FAIL;
                                        row[x] = nx_rgb_to_gray_uc(value[0], value[1], value[2]);
                                }
                        }
                } else if (img_type == NX_IMAGE_RGBA) {
                        for (int y = 0; y < img->height; ++y) {
                                uchar* row = img->data.uc + y * img->row_stride;
                                for (int x = 0; x < img->width; ++x) {
                                        if (fread((void*)(row + 4*x), sizeof(uchar), 3, pnm) != 3)
                                                return NX_FAIL;
                                        row[4*x+3] = 255;
                                }
                        }
                } else {
                        NX_FATAL(NX_LOG_TAG, "Unsupported image type while loading RGB PNM!");
                }
        }

        return NX_OK;
}

void nx_image_xload_pnm(struct NXImage *img, const char *filename,
                        enum NXImageLoadMode mode)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(filename);
        NX_ASSERT_CUSTOM("Only UCHAR images can be loaded directly from PNM files", img->dtype==NX_IMAGE_UCHAR);

        FILE *pnm = nx_xfopen(filename, "rb");

        char ch1, ch2;
        if (fscanf(pnm, "%c%c", &ch1, &ch2) != 2) {
                NX_IO_FATAL(NX_LOG_TAG, "Could not read image header from %s", filename);
        }

        if(ch1 != 'P' || (ch2 != '5' && ch2 != '6')) {
                NX_FATAL(NX_LOG_TAG, "Image %s is not a PNM file", filename);
        }

        enum NXImageType pnm_type;
        if(ch2 == '5') {
                pnm_type = NX_IMAGE_GRAYSCALE;
        } else if(ch2 == '6') {
                pnm_type = NX_IMAGE_RGBA;
        }

        int c = fgetc(pnm);
        while (c == ' ' || c == '\n' || c == '\t') {
                c = fgetc(pnm);
        }

        while (c == '#') {
                while (c != '\n') {
                        c = fgetc(pnm);
                        if (c == EOF)
                                NX_IO_FATAL(NX_LOG_TAG, "%s does not contain image data", filename);
                }
                c = fgetc(pnm);
        }
        ungetc(c, pnm);

        int pnm_width;
        int pnm_height;
        int pnm_levels;
        if (fscanf(pnm, "%d %d %d", &pnm_width, &pnm_height, &pnm_levels) != 3) {
                NX_IO_FATAL(NX_LOG_TAG, "Could not read image attributes from %s", filename);
        }
        c = fgetc(pnm);

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

        if (read_pnm_data(img, img_type, pnm, pnm_width, pnm_height, pnm_type) != NX_OK)
                NX_IO_FATAL(NX_LOG_TAG, "Error reading PNM data from %s", filename);

        nx_xfclose(pnm, filename);
}

NXResult nx_image_load_pnm(struct NXImage *img, const char *filename,
                           enum NXImageLoadMode mode)
{
        NX_ASSERT_PTR(img);
        NX_ASSERT_PTR(filename);
        NX_ASSERT_CUSTOM("Only UCHAR images can be loaded directly from PNM files", img->dtype==NX_IMAGE_UCHAR);

        FILE *pnm = nx_fopen(filename, "rb");
        if (!pnm) {
                return NX_FAIL;
        }

        char ch1, ch2;
        if (fscanf(pnm, "%c%c", &ch1, &ch2) != 2) {
                NX_IO_ERROR(NX_LOG_TAG, "Could not read image header from %s", filename);
                fclose(pnm);
                return NX_FAIL;
        }

        if(ch1 != 'P' || (ch2 != '5' && ch2 != '6')) {
                NX_ERROR(NX_LOG_TAG, "Image %s is not a PNM file", filename);
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
                NX_IO_ERROR(NX_LOG_TAG, "Could not read image attributes from %s", filename);
                fclose(pnm);
                return NX_FAIL;
        }

        // Skip a new line
        char buffer[256];
        if (fgets(buffer, sizeof(buffer), pnm) == NULL) {
                NX_IO_ERROR(NX_LOG_TAG, "Error reading from %s", filename);
                fclose(pnm);
                return NX_FAIL;
        }


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

        NXResult res_read = read_pnm_data(img, img_type, pnm, pnm_width, pnm_height, pnm_type);
        NXResult res_close = nx_fclose(pnm, filename);

        if (res_read == NX_OK)
                return res_close;
        else
                return res_read;
}
