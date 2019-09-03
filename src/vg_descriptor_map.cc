/**
 * @file vg_descriptor_map.cc
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
#include "virg/nexus/vg_descriptor_map.hpp"

#include <cstring>

#include "virg/nexus/vg_brief_extractor.hpp"

using std::memcpy;
using std::vector;

namespace virg {
namespace nexus {

VGDescriptorMap::VGDescriptorMap(int n_octets)
        : m_n_octets(n_octets)
{}

void VGDescriptorMap::clear()
{
        m_ids.clear();
        m_descriptor_data.clear();
}

void VGDescriptorMap::reserve(int n_descriptors)
{
        m_ids.reserve(n_descriptors);
        m_descriptor_data.reserve(n_descriptors*m_n_octets);
}

int VGDescriptorMap::add(int id, const uchar* desc)
{
        int i = static_cast<int>(m_ids.size());
        m_ids.push_back(id);

        m_descriptor_data.resize(m_descriptor_data.size() + m_n_octets);
        uchar* desc_i = m_descriptor_data.data() + i*m_n_octets;
        memcpy(desc_i, desc, m_n_octets*sizeof(*desc));

        return static_cast<int>(m_ids.size());
}

VGDescriptorMap::SearchResult VGDescriptorMap::search_nn(const uchar* desc)
{
        int n_desc = static_cast<int>(m_ids.size());
        uchar* desc_ptr = &m_descriptor_data[0];
        int id = 0;
        int dist = VGBriefExtractor::distance_of(m_n_octets, desc, desc_ptr);
        for (int i = 1; i < n_desc; ++i) {
                int d = VGBriefExtractor::distance_of(m_n_octets, desc,
                                                      desc_ptr+i*m_n_octets);
                if (d < dist) {
                        dist = d;
                        id = i;
                }
        }

        SearchResult r;
        r.ids.push_back(id);
        r.distances.push_back(dist);
        return r;
}

const uchar* VGDescriptorMap::search_by_id(int id)
{
        int n_desc = static_cast<int>(m_ids.size());
        for (int i = 0; i < n_desc; ++i) {
                if (id == m_ids[i])
                        return m_descriptor_data.data() + i*m_n_octets;
        }

        return nullptr;
}

}
}
