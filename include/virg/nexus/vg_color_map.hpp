/**
 * @file vg_color_map.hpp
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
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_VG_COLOR_MAP_HPP
#define VIRG_NEXUS_VG_COLOR_MAP_HPP

#include "virg/nexus/vg_color.hpp"

namespace virg {
namespace nexus {

class VGColorMap {
public:
        enum Type { GRAY = 0, VIRIDIS, PLASMA,
                    MAGMA, CUBEHELIX };

        static inline VGColor apply(Type type, float value) {
                uchar v[3];
                nx_color_map_apply(&v[0], &v[1], &v[2], value,
                                   VGColorMap::type_to_nx_type(type));
                return VGColor(v[0]/255.0, v[1]/255.0, v[2]/255.0, 255);
        }

        static inline enum NXColorMap type_to_nx_type(Type type) {
                switch (type) {
                case GRAY:      return  NX_COLOR_MAP_GRAY;
                case VIRIDIS:   return NX_COLOR_MAP_VIRIDIS;
                case PLASMA:    return NX_COLOR_MAP_PLASMA;
                case MAGMA:     return NX_COLOR_MAP_MAGMA;
                case CUBEHELIX: return NX_COLOR_MAP_CUBEHELIX;
                default:
                        NX_FATAL(VG_LOG_TAG, "Unknown VGColorMap type!");
                }
        }
};

}
}

#endif
