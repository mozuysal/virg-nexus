/**
 * @file vg_harris_detector.hpp
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

        int detect(const VGImage& image, int max_n_keys, bool adapt_threshold);
        int detect_pyr(const VGImagePyr& pyr, int n_key_levels,
                       int max_n_keys, bool adapt_threshold);

        std::vector<NXKeypoint>& keys();
        const std::vector<NXKeypoint>& keys() const;
private:
        void update_score_image(const VGImage& image);
        float adapt_threshold(float threshold, int n_keys, int max_n_keys);

        float m_sigma_win;
        float m_k;
        float m_threshold;

        VGImage m_dimg[3];
        VGImage m_simg;

        std::vector<NXKeypoint> m_keys;
};

}
}

#endif
