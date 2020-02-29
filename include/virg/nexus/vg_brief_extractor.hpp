/**
 * @file vg_brief_extractor.hpp
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
 * You should have received a copy of the GNU General Public License along with
 * VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_VG_BRIEF_EXTRACTOR_HPP
#define VIRG_NEXUS_VG_BRIEF_EXTRACTOR_HPP

#include <memory>
#include <virg/nexus/nx_keypoint.h>
#include <virg/nexus/nx_brief_extractor.h>
#include <virg/nexus/vg_image_pyr.hpp>
#include "virg/nexus/vg_descriptor_map.hpp"

namespace virg {
namespace nexus {

class VGBriefExtractor {
public:
        VGBriefExtractor(int n_octets = 32, int radius = 24);
        ~VGBriefExtractor() {}

        VGBriefExtractor& resize(int n_octets, int radius);
        VGBriefExtractor& randomize_with_seed(uint32_t seed);
        VGBriefExtractor& randomize();

        bool check_point_pyr   (const VGImagePyr& pyr, int x, int y, int level);
        bool check_keypoint_pyr(const VGImagePyr& pyr, const struct NXKeypoint& key);

        void compute_pyr    (const VGImagePyr& pyr, int x, int y, int level,
                             uchar *desc);
        void compute_pyr    (const VGImagePyr& pyr, const struct NXKeypoint& key,
                             uchar *desc);
        void compute_pyr    (const VGImagePyr& pyr, int n_keys, const struct NXKeypoint* keys,
                             VGDescriptorMap& desc_map);

        static inline int distance_of(int n_octets, const uchar *desc0, const uchar *desc1) {
                return nx_brief_extractor_descriptor_distance(n_octets, desc0, desc1);
        }
private:
        void pick_good_seed();

        std::shared_ptr<NXBriefExtractor> m_be;
};

inline VGBriefExtractor& VGBriefExtractor::randomize_with_seed(uint32_t seed)
{
        nx_brief_extractor_randomize_with_seed(m_be.get(), seed);
        return *this;
}

inline VGBriefExtractor& VGBriefExtractor::randomize()
{
        nx_brief_extractor_randomize(m_be.get());
        return *this;
}

inline bool VGBriefExtractor::check_point_pyr(const VGImagePyr& pyr,
                                       int x, int y, int level)
{
        return nx_brief_extractor_check_point_pyr(m_be.get(), pyr.nx_pyr(),
                                                  x, y, level);
}

inline bool VGBriefExtractor::check_keypoint_pyr(const VGImagePyr& pyr,
                                                 const struct NXKeypoint& key)
{
        return check_point_pyr(pyr, key.x, key.y, key.level);
}

inline void VGBriefExtractor::compute_pyr(const VGImagePyr& pyr,
                                   int x, int y, int level,
                                   uchar *desc)
{
        nx_brief_extractor_compute_pyr(m_be.get(), pyr.nx_pyr(), x, y, level, desc);
}

inline void VGBriefExtractor::compute_pyr(const VGImagePyr& pyr,
                                          const struct NXKeypoint& key,
                                          uchar *desc)
{
        nx_brief_extractor_compute_pyr(m_be.get(), pyr.nx_pyr(), key.x, key.y,
                                       key.level, desc);
}

}
}

#endif
