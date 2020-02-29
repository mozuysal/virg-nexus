/**
 * @file vg_harris_detector.hpp
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
#ifndef VIRG_NEXUS_VG_HARRIS_DETECTOR_HPP
#define VIRG_NEXUS_VG_HARRIS_DETECTOR_HPP

#include <vector>

#include "virg/nexus/nx_keypoint.h"
#include "virg/nexus/vg_image.hpp"
#include "virg/nexus/vg_image_pyr.hpp"

namespace virg {
namespace nexus {

class VGHarrisDetector {
public:
        VGHarrisDetector();
        ~VGHarrisDetector();

        void set_sigma_win(float sigma_win);
        void set_k(float k);
        void set_threshold(float threshold);

        int detect(const VGImage& image, std::vector<struct NXKeypoint> &keys,
                   int max_n_keys, bool adapt_threshold);
        int detect_pyr(const VGImagePyr& pyr, std::vector<struct NXKeypoint> &keys,
                       int n_key_levels, int max_n_keys, bool adapt_threshold);
private:
        void update_score_image(const VGImage& image);
        float adapt_threshold(float threshold, int n_keys, int max_n_keys);

        float m_sigma_win;
        float m_k;
        float m_threshold;

        VGImage m_dimg[3];
        VGImage m_simg;
};

}
}

#endif
