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

#include "virg/nexus/nx_types.h"

namespace virg {
namespace nexus {

class VGDescriptorMap {
public:
        struct SearchResult {
                std::vector<int>   ids;
                std::vector<float> distances;
        };

        VGDescriptorMap(int n_octets);

        void clear();
        void reserve(int n_descriptors);

        int size    () const { return static_cast<int>(m_ids.size()); }
        int n_octets() const { return m_n_octets; }

        int add(int id, const uchar* desc);

        /**
         *  Brute force search for nearest neighbor in hamming distance.
         */
        SearchResult search_nn(const uchar* desc);
        const uchar* search_by_id(int id);

private:
        int m_n_octets;
        std::vector<int>   m_ids;
        std::vector<uchar> m_descriptor_data;
};

}
}

#endif
