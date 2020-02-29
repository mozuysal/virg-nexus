/**
 * @file vg_color.hpp
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
#ifndef VIRG_NEXUS_VG_COLOR_HPP
#define VIRG_NEXUS_VG_COLOR_HPP

#include "nx_uniform_sampler.h"

namespace virg {
namespace nexus {

class VGColor {
public:
        VGColor(double red, double green, double blue, double alpha)
                : m_color{red, green, blue, alpha} {}

        static inline VGColor red() { return VGColor(1.0, 0.0, 0.0, 1.0); }
        static inline VGColor green() { return VGColor(0.0, 1.0, 0.0, 1.0); }
        static inline VGColor blue() { return VGColor(0.0, 0.0, 1.0, 1.0); }
        static inline VGColor black() { return VGColor(0.0, 0.0, 0.0, 1.0); }
        static inline VGColor white() { return VGColor(1.0, 1.0, 1.0, 1.0); }
        static inline VGColor yellow() { return VGColor(1.0, 1.0, 0.0, 1.0); }
        static inline VGColor purple() { return VGColor(1.0, 0.0, 1.0, 1.0); }
        static inline VGColor cyan() { return VGColor(0.0, 1.0, 1.0, 1.0); }

        static inline VGColor random() {
                return VGColor(NX_UNIFORM_SAMPLE_D, NX_UNIFORM_SAMPLE_D,
                               NX_UNIFORM_SAMPLE_D, NX_UNIFORM_SAMPLE_D);
        }

        double r() const { return m_color[0]; }
        double g() const { return m_color[1]; }
        double b() const { return m_color[2]; }
        double a() const { return m_color[3]; }

        const double* v() const { return &m_color[0]; }
private:
        double m_color[4];
};

}
}

#endif
