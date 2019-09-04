/**
 * @file vg_brief_extractor.cc
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
#include "virg/nexus/vg_brief_extractor.hpp"

using std::shared_ptr;
using std::unique_ptr;

namespace virg {
namespace nexus {

VGBriefExtractor::VGBriefExtractor(int n_octets, int radius)
        : m_be(nx_brief_extractor_new(n_octets, radius),
               nx_brief_extractor_free)
{}

VGBriefExtractor& VGBriefExtractor::resize(int n_octets, int radius)
{
        shared_ptr<NXBriefExtractor> ptr(nx_brief_extractor_new(n_octets, radius),
                                         nx_brief_extractor_free);
        m_be = ptr;
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
