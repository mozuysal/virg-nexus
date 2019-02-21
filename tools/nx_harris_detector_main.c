/**
 * @file nx_harris_detector_main.c
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2019 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include <stdlib.h>

#include "virg/nexus/nx_options.h"
#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_image.h"
#include "virg/nexus/nx_harris_detector.h"
#include "virg/nexus/nx_gc.h"
#include "virg/nexus/nx_json_tree.h"
#include "virg/nexus/nx_json_bundle.h"

int main(int argc, char** argv)
{
        struct NXOptions* opt = nx_options_new("Sddidsbb",
                                               "-i", "input IMAGE", "",
                                               "--sigma-int", "integration scale", 1.2,
                                               "-k", "Harris corner score parameter", 0.06,
                                               "-N", "maximum number of keypoints", 2000,
                                               "-t", "threshold for acceptable keypoint scores", 0.00005,
                                               "--key-image", "draw keypoints and save the image to named file", NULL,
                                               "--debug", "save debug information to /tmp", NX_FALSE,
                                               "-v|--verbose", "log more information to stderr", NX_FALSE);
        nx_options_add_help(opt);
        nx_options_set_usage_header(opt, "Detects Harris corner points.\n\n");
        nx_options_set_usage_footer(opt, "\nCopyright (C) 2019 Mustafa Ozuysal.\n");

        nx_options_set_from_args(opt, argc, argv);
        const char* input_name = nx_options_get_string(opt, "-i");
        float sigma_win = nx_options_get_double(opt, "--sigma-int");
        float k = nx_options_get_double(opt, "-k");
        double threshold = nx_options_get_double(opt, "-t");
        int n_keys_max = nx_options_get_int(opt, "-N");
        const char* key_image = nx_options_get_string(opt, "--key-image");
        NXBool is_debug = nx_options_get_bool(opt, "--debug");
        NXBool is_verbose = nx_options_get_bool(opt, "-v");

        if (is_verbose)
                nx_options_print_values(opt, stderr);

        struct NXImage* img = nx_image_alloc();
        nx_image_xload(img, input_name, NX_IMAGE_LOAD_GRAYSCALE);

        if (is_verbose)
                NX_LOG("HARRIS", "Loaded image of size %dx%d from file %s.",
                       img->width, img->height, input_name);

        struct NXImage** dimg = NX_NEW(3, struct NXImage*);
        for (int i = 0; i < 3; ++i)
                dimg[i] = nx_image_alloc();
        nx_harris_deriv_images(dimg, img, sigma_win);

        struct NXImage* simg = nx_image_alloc();
        nx_harris_score_image(simg, dimg, k);

        struct NXKeypoint* keys = NX_NEW(n_keys_max, struct NXKeypoint);
        int n_keys = nx_harris_detect_keypoints(n_keys_max, keys, simg, threshold);

        if (is_verbose)
                NX_LOG("HARRIS", "Detected %d keypoints.", n_keys);

        struct NXJSONNode* jopt = nx_json_node_new_object();
        nx_json_object_add(jopt, "image", nx_json_bundle_string(input_name));
        nx_json_object_add(jopt, "integration-scale", nx_json_bundle_float(sigma_win));
        nx_json_object_add(jopt, "k", nx_json_bundle_double(k));
        nx_json_object_add(jopt, "threshold", nx_json_bundle_double(threshold));
        nx_json_object_add(jopt, "max-n-keys", nx_json_bundle_int(n_keys_max));
        struct NXJSONNode* jkeys = nx_json_bundle_keypoint_array(n_keys, keys);
        struct NXJSONNode* jout = nx_json_node_new_object();
        nx_json_object_add(jout, "options", jopt);
        nx_json_object_add(jout, "keypoints", jkeys);
        nx_json_tree_print(jout, 2);
        nx_json_tree_free(jout);

        if (key_image != NULL) {
                struct NXImage* kimg = nx_image_copy0(img);
                nx_image_convert_type(kimg, NX_IMAGE_RGBA);
                struct NXGC* gc = nx_gc_new(kimg);
                nx_gc_set_color(gc, 0.0, 1.0, 0.0, 1.0);
                for (int i = 0; i < n_keys; ++i) {
                        double x = keys[i].x;
                        double y = keys[i].y;
                        nx_gc_new_path(gc);
                        nx_gc_circle(gc, x, y, 3);
                        nx_gc_stroke(gc);
                }
                nx_gc_flush(gc);
                nx_image_xsave(kimg, key_image);
                nx_gc_free(gc);
                nx_image_free(kimg);
        }

        if (is_debug) {
                struct NXImage* debug_img = nx_image_alloc();

                const char* score_image_filename = "/tmp/harris_score.ppm";
                nx_image_normalize_to_zero_one(simg, NX_FALSE);
                nx_image_apply_colormap(debug_img, simg, NX_COLOR_MAP_CUBEHELIX);
                nx_image_xsave_pnm(debug_img, score_image_filename);

                const char* x2_image_filename = "/tmp/harris_deriv_x2.ppm";
                nx_image_normalize_to_zero_one(dimg[0], NX_FALSE);
                nx_image_apply_colormap(debug_img, dimg[0], NX_COLOR_MAP_CUBEHELIX);
                nx_image_xsave_pnm(debug_img, x2_image_filename);

                const char* y2_image_filename = "/tmp/harris_deriv_y2.ppm";
                nx_image_normalize_to_zero_one(dimg[1], NX_FALSE);
                nx_image_apply_colormap(debug_img, dimg[1], NX_COLOR_MAP_CUBEHELIX);
                nx_image_xsave_pnm(debug_img, y2_image_filename);

                const char* xy_image_filename = "/tmp/harris_deriv_xy.ppm";
                nx_image_normalize_to_zero_one(dimg[2], NX_FALSE);
                nx_image_apply_colormap(debug_img, dimg[2], NX_COLOR_MAP_CUBEHELIX);
                nx_image_xsave_pnm(debug_img, xy_image_filename);

                nx_image_free(debug_img);
        }

        nx_free(keys);
        nx_image_free(simg);
        for (int i = 0; i < 3; ++i)
                nx_image_free(dimg[i]);
        nx_free(dimg);
        nx_image_free(img);
        nx_options_free(opt);

        return EXIT_SUCCESS;
}
