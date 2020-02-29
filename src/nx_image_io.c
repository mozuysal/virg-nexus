/**
 * @file nx_image_io.c
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
#include "virg/nexus/nx_image_io.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_assert.h"

enum NXImageFileFormat nx_image_file_format_from_header(const char *filename)
{
        FILE *img_stream = fopen(filename, "rb");
        if (img_stream == NULL) {
                NX_ERROR(NX_LOG_TAG, "Error opening file %s for image format check!", filename);
                return NX_IMAGE_FILE_FORMAT_UNKNOWN;
        }

        uchar magic[4];
        if (fread(&magic[0], sizeof(magic[0]), 4, img_stream) != 4) {
                NX_ERROR(NX_LOG_TAG, "Error reading image header from file %s!", filename);
                fclose(img_stream);
                return NX_IMAGE_FILE_FORMAT_UNKNOWN;
        }


        enum NXImageFileFormat fmt = NX_IMAGE_FILE_FORMAT_UNKNOWN;
        if (magic[0] == 'P') {
                if (magic[1] == '4' || magic[1] == '5' || magic[1] == '6')
                        fmt = NX_IMAGE_FILE_FORMAT_PNM_BINARY;
                else if (magic[1] == '1' || magic[1] == '2' || magic[1] == '3')
                        fmt = NX_IMAGE_FILE_FORMAT_PNM_ASCII;
        }

        if (magic[0] == 0xFF && magic[1] == 0xD8)
                fmt = NX_IMAGE_FILE_FORMAT_JPEG;

        if (magic[0] == 0x89 && magic[1] == 0x50
            && magic[2] == 0x4E && magic[3] == 0x47)
                fmt = NX_IMAGE_FILE_FORMAT_PNG;

        fclose(img_stream);

        return fmt;
}

enum NXImageFileFormat nx_image_file_format_from_filename(const char *filename)
{
        const char *dot = strrchr(filename, '.');
        if (dot == NULL)
                return NX_IMAGE_FILE_FORMAT_UNKNOWN;
        else if ((strcmp(dot, ".pgm") == 0) || (strcmp(dot, ".PGM") == 0)
                 || (strcmp(dot, ".ppm") == 0) || (strcmp(dot, ".PPM") == 0))
                return NX_IMAGE_FILE_FORMAT_PNM_BINARY;
        else if ((strcmp(dot, ".jpg") == 0) || (strcmp(dot, ".jpeg") == 0)
                 || (strcmp(dot, ".JPG") == 0) || (strcmp(dot, ".JPEG") == 0))
                return NX_IMAGE_FILE_FORMAT_JPEG;
        else if ((strcmp(dot, ".png") == 0) || (strcmp(dot, ".PNG") == 0))
                return NX_IMAGE_FILE_FORMAT_PNG;
        else
                return NX_IMAGE_FILE_FORMAT_UNKNOWN;
}

void nx_image_xsave(const struct NXImage *img, const char *filename)
{
        if (nx_image_save(img, filename) != NX_OK)
                NX_FATAL(NX_LOG_TAG, "Error saving image to %s!", filename);
}

void nx_image_xload(struct NXImage *img, const char *filename, enum NXImageLoadMode mode)
{
        if (nx_image_load(img, filename, mode) != NX_OK)
                NX_FATAL(NX_LOG_TAG, "Error loading image from %s!", filename);
}

NXResult nx_image_save(const struct NXImage *img, const char *filename)
{
        enum NXImageFileFormat fmt = nx_image_file_format_from_filename(filename);

        switch (fmt) {
        case NX_IMAGE_FILE_FORMAT_PNM_BINARY:
                return nx_image_save_pnm(img, filename);
        case NX_IMAGE_FILE_FORMAT_JPEG:
                return nx_image_save_jpeg(img, filename);
        case NX_IMAGE_FILE_FORMAT_PNG:
                return nx_image_save_png(img, filename);
        default:
                NX_WARNING(NX_LOG_TAG, "Unknown/Unsupported file format: Trying to save image as PNM to %s", filename);
        }

        return nx_image_save_pnm(img, filename);
}

NXResult nx_image_load(struct NXImage *img, const char *filename, enum NXImageLoadMode mode)
{
        enum NXImageFileFormat fmt = nx_image_file_format_from_header(filename);

        switch (fmt) {
        case NX_IMAGE_FILE_FORMAT_PNM_ASCII:
                NX_ERROR(NX_LOG_TAG, "Loading of ASCII PNM files is not supported");
                return NX_FAIL;
        case NX_IMAGE_FILE_FORMAT_PNM_BINARY:
                return nx_image_load_pnm(img, filename, mode);
        case NX_IMAGE_FILE_FORMAT_JPEG:
                return nx_image_load_jpeg(img, filename, mode);
        case NX_IMAGE_FILE_FORMAT_PNG:
                return nx_image_load_png(img, filename, mode);
        default:
                NX_ERROR(NX_LOG_TAG, "Unknown file format: Trying to load image from %s", filename);
                return NX_FAIL;
        }
}
