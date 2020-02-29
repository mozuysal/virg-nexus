/**
 * @file vg_image_pyr.hpp
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
#ifndef VIRG_NEXUS_VG_IMAGE_PYR_HPP
#define VIRG_NEXUS_VG_IMAGE_PYR_HPP

#include <memory>
#include <vector>

#include "virg/nexus/nx_image_pyr.h"
#include "virg/nexus/nx_image_pyr_builder.h"

#include "virg/nexus/vg_image.hpp"

namespace virg {
namespace nexus {

class VGImagePyr {
public:
        enum Type { FAST, FINE, SCALED };
        VGImagePyr();
        ~VGImagePyr();

        static VGImagePyr build_fast_from  (const VGImage& image, int n_levels, float sigma0);
        static VGImagePyr build_fine_from  (const VGImage& image, int n_octaves, int n_octave_steps, float sigma0);
        static VGImagePyr build_scaled_from(const VGImage& image, int n_levels, float scale_factor, float sigma0);

        void rebuild();
        void rebuild_from(const VGImage& image);

        Type  type          () const;
        int   n_levels      () const { return m_pyr->n_levels; }
        int   n_octaves     () const { return m_builder->pyr_info.n_octaves; }
        int   n_octave_steps() const { return m_builder->pyr_info.n_octave_steps; }
        float scale_factor  () const { return m_builder->pyr_info.scale_factor; }
        float sigma0        () const { return m_builder->pyr_info.sigma0; }

        float         level_sigma(int idx) const { return m_pyr->levels[idx].sigma; }
        float         level_scale(int idx) const { return m_pyr->levels[idx].scale; }
        VGImage       operator[] (int idx)       { VGImage l(m_pyr->levels[idx].img, false); return l; }
        const VGImage operator[] (int idx) const { VGImage l(m_pyr->levels[idx].img, false); return l; }

        const struct NXImagePyr* nx_pyr() const { return m_pyr.get(); }
        struct NXImagePyr*       nx_pyr()       { return m_pyr.get(); }
private:
        std::shared_ptr<struct NXImagePyr> m_pyr;
        std::shared_ptr<struct NXImagePyrBuilder> m_builder;
};

}
}

#endif
