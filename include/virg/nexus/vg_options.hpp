/**
 * @file vg_options.hpp
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
#ifndef VIRG_NEXUS_VG_OPTIONS_HPP
#define VIRG_NEXUS_VG_OPTIONS_HPP

#include <string>
#include <vector>
#include <memory>

#include "virg/nexus/nx_options.h"

namespace virg {
namespace nexus {

class VGOptions {
public:
        enum RequirementState { NOT_REQUIRED=0, REQUIRED };

        VGOptions();
        ~VGOptions();

        void add_bool  (const std::string& switches, const std::string& help_text,  enum VGOptions::RequirementState is_required, bool defaut_value = false);
        void add_int   (const std::string& switches, const std::string& help_text,  enum VGOptions::RequirementState is_required, int defaut_value = 0);
        void add_double(const std::string& switches, const std::string& help_text,  enum VGOptions::RequirementState is_required, double defaut_value = 0.0);
        void add_string(const std::string& switches, const std::string& help_text,  enum VGOptions::RequirementState is_required, const std::string& defaut_value = "");
        void add_rest  (const std::string& help_text,  enum VGOptions::RequirementState is_required);
        void add_help  ();

        void set_from_args(int argc, char **argv);

        bool        get_bool  (const std::string& opt_name);
        int         get_int   (const std::string& opt_name);
        double      get_double(const std::string& opt_name);
        std::string get_string(const std::string& opt_name);

        std::vector<std::string> get_rest();

        bool is_set(const std::string& opt_name);

        void print_values(FILE* stream);
        void print_usage(FILE* stream);
        void set_usage_header(const std::string& header);
        void set_usage_footer(const std::string& footer);

private:
        std::shared_ptr<struct NXOptions> m_opt;
};

}
}

#endif
