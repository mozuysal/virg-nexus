/**
 * @file vg_brief_extractor.cc
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
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/vg_brief_extractor.hpp"

using std::shared_ptr;
using std::unique_ptr;

namespace virg {
namespace nexus {

void VGBriefExtractor::pick_good_seed()
{
        if (m_be->radius == 16) {
                nx_brief_extractor_randomize_with_seed(m_be.get(),
                                                       NX_BRIEF_EXTRACTOR_GOOD_SEED_N32_R16);
        } else {
                nx_brief_extractor_randomize_with_seed(m_be.get(),
                                                       NX_BRIEF_EXTRACTOR_GOOD_SEED_N32_R24);
        }
}

VGBriefExtractor::VGBriefExtractor(int n_octets, int radius)
        : m_be(nx_brief_extractor_new(n_octets, radius),
               nx_brief_extractor_free)
{
        pick_good_seed();
}

VGBriefExtractor& VGBriefExtractor::resize(int n_octets, int radius)
{
        shared_ptr<NXBriefExtractor> ptr(nx_brief_extractor_new(n_octets, radius),
                                         nx_brief_extractor_free);
        m_be = ptr;
        pick_good_seed();
        return *this;
}

void VGBriefExtractor::compute_pyr(const VGImagePyr& pyr, int n_keys,
                                   const struct NXKeypoint* keys,
                                   VGDescriptorMap& desc_map)
{
        NX_ASSERT(desc_map.n_octets() == m_be->n_octets);

        unique_ptr<uchar[]> desc(new uchar[m_be->n_octets]);
        for (int i = 0; i < n_keys; ++i) {
                if (check_keypoint_pyr(pyr, keys[i])) {
                        compute_pyr(pyr, keys[i], desc.get());
                        desc_map.add(keys[i].id, desc.get());
                }
        }
}

}
}
