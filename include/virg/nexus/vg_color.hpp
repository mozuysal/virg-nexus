/**
 * @file vg_color.hpp
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
#ifndef VIRG_NEXUS_VG_COLOR_HPP
#define VIRG_NEXUS_VG_COLOR_HPP

#include "nx_uniform_sampler.h"

namespace virg {
namespace nexus {

class VGColor {
public:
        VGColor(double red, double green, double blue, double alpha);

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
