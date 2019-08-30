/**
 * @file vg_stereo_matcher_main.cc
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
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <ctime>

#include "virg/nexus/nx_log.h"

#include "virg/nexus/vg_options.hpp"
#include "virg/nexus/vg_image.hpp"
#include "virg/nexus/vg_harris_detector.hpp"

using namespace std;

using virg::nexus::VGOptions;
using virg::nexus::VGImage;
using virg::nexus::VGHarrisDetector;

static const char* LOG_TAG = "STEREO";
static const int MAX_N_KEYS = 2000;

int main(int argc, char** argv)
{
        VGOptions opt;
        opt.add_string("-l", "image file for the left image.", VGOptions::REQUIRED, "");
        opt.add_string("-r", "image file for the right image.", VGOptions::REQUIRED, "");
        opt.add_bool("-v|--verbose", "display more information.", VGOptions::NOT_REQUIRED, false);
        opt.add_help();
        opt.set_from_args(argc, argv);

        string left_image = opt.get_string("-l");
        string right_image = opt.get_string("-r");
        bool is_verbose = opt.get_bool("-v");

        VGImage images[2];
        images[0].xload(left_image, VGImage::LOAD_GRAYSCALE);
        images[1].xload(right_image, VGImage::LOAD_GRAYSCALE);

        if (is_verbose) {
                NX_LOG(LOG_TAG, "Loaded %dx%d image from %s.", images[0].width(), images[0].height(), left_image.c_str());
                NX_LOG(LOG_TAG, "Loaded %dx%d image from %s.", images[1].width(), images[1].height(), right_image.c_str());
        }

        VGHarrisDetector detector[2];
        for (int i = 0; i < 2; ++i) {
                if (is_verbose) NX_LOG(LOG_TAG, "Processing image %d/2", i+1);
                int n_keys;
                for (int k = 0; k < 20; ++k) {
                        n_keys = detector[i].detect(images[i], MAX_N_KEYS, true);
                        if (n_keys < MAX_N_KEYS && n_keys >= 0.95*MAX_N_KEYS)
                                break;
                }
                if (is_verbose) NX_LOG(LOG_TAG, "  Detected %d keypoints", n_keys);
        }

        return EXIT_SUCCESS;
}

/// Local Variables:
/// mode: c++
/// compile-command: "make vg-stereo-matcher"
/// End:
