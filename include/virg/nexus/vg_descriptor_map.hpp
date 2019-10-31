/**
 * @file vg_descriptor_map.hpp
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
 * You should have received a copy of the GNU General Public License
 * along with VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
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

        int      size  ()        const { return static_cast<int>(m_ids.size()); }
        uint64_t get_id(int idx) const { return m_ids[idx]; }

        uchar*       get_descriptor(int idx)       { return m_descriptor_data.data() + idx*m_n_octets; }
        const uchar* get_descriptor(int idx) const { return m_descriptor_data.data() + idx*m_n_octets; }

        int  n_octets    () const { return m_n_octets; }
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
