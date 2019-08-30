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

#include "virg/nexus/vg_options.hpp"

using namespace std;

using virg::nexus::VGOptions;

int main(int argc, char** argv)
{
        VGOptions opt;

        opt.add_string("-l", "image file for the left image.", VGOptions::REQUIRED, "");
        opt.add_string("-r", "image file for the right image.", VGOptions::REQUIRED, "");
        opt.add_bool("-v|--verbose", "display more information.", VGOptions::NOT_REQUIRED, false);
        opt.add_help();

        opt.set_from_args(argc, argv);

        return EXIT_SUCCESS;
}

/// Local Variables:
/// mode: c++
/// compile-command: "make vg-stereo-matcher"
/// End:
