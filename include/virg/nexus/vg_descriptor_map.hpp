/**
 * @file vg_descriptor_map.hpp
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
#ifndef VIRG_NEXUS_VG_DESCRIPTOR_MAP_HPP
#define VIRG_NEXUS_VG_DESCRIPTOR_MAP_HPP

#include <vector>
#include <tuple>

#include "virg/nexus/nx_types.h"

namespace virg {
namespace nexus {

class VGDescriptorMap {
public:
        struct SearchResult {
                uint64_t id;
                float match_cost;
        };

        VGDescriptorMap(int n_octets=32);

        void clear();
        void reserve(int n_descriptors);

        int size    () const { return static_cast<int>(m_ids.size()); }
        int n_octets() const { return m_n_octets; }
        void set_n_octets(int n_octets) { this->clear(); m_n_octets = n_octets; }

        int add(uint64_t id, const uchar* desc);

        /**
         *  Brute force search for nearest neighbor in hamming distance.
         */
        SearchResult search_nn(const uchar* desc);
        const uchar* search_by_id(uint64_t id);
private:
        int m_n_octets;
        std::vector<uint64_t> m_ids;
        std::vector<uchar>    m_descriptor_data;
};

}
}

#endif
