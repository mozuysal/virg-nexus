/**
 * @file vg_options.cc
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
#include "virg/nexus/vg_options.hpp"

using std::string;
using std::vector;
using std::shared_ptr;

namespace virg {
namespace nexus {

VGOptions::VGOptions()
        : m_opt(nx_options_alloc(), nx_options_free)
{}

VGOptions::~VGOptions()
{}

void VGOptions::add_bool(const std::string& switches, const std::string& help_text,  enum VGOptions::RequirementState is_required, bool defaut_value)
{
        const string opt_format = (is_required == VGOptions::REQUIRED) ? "B" : "b";
        NXBool def_val = static_cast<NXBool>(defaut_value);
        nx_options_add(m_opt.get(), opt_format.c_str(), switches.c_str(), help_text.c_str(), def_val);
}

void VGOptions::add_int(const std::string& switches, const std::string& help_text,  enum VGOptions::RequirementState is_required, int defaut_value)
{
        const string opt_format = (is_required == VGOptions::REQUIRED) ? "I" : "i";
        nx_options_add(m_opt.get(), opt_format.c_str(), switches.c_str(), help_text.c_str(), defaut_value);
}

void VGOptions::add_double(const std::string& switches, const std::string& help_text,  enum VGOptions::RequirementState is_required, double defaut_value)
{
        const string opt_format = (is_required == VGOptions::REQUIRED) ? "D" : "d";
        nx_options_add(m_opt.get(), opt_format.c_str(), switches.c_str(), help_text.c_str(), defaut_value);
}

void VGOptions::add_string(const std::string& switches, const std::string& help_text,  enum VGOptions::RequirementState is_required, const std::string& defaut_value)
{
        const string opt_format = (is_required == VGOptions::REQUIRED) ? "S" : "s";
        nx_options_add(m_opt.get(), opt_format.c_str(), switches.c_str(), help_text.c_str(), defaut_value.c_str());
}

void VGOptions::add_rest(const std::string& help_text,  enum VGOptions::RequirementState is_required)
{
        const string opt_format = (is_required == VGOptions::REQUIRED) ? "R" : "r";
        nx_options_add(m_opt.get(), opt_format.c_str(), help_text.c_str());
}

void VGOptions::add_help()
{
        nx_options_add_help(m_opt.get());
}

void VGOptions::set_from_args(int argc, char **argv)
{
        nx_options_set_from_args(m_opt.get(), argc, argv);
}

bool VGOptions::get_bool(const std::string& opt_name)
{
        NXBool r = nx_options_get_bool(m_opt.get(), opt_name.c_str());
        return static_cast<bool>(r);
}

int VGOptions::get_int(const std::string& opt_name)
{
        return nx_options_get_int(m_opt.get(), opt_name.c_str());
}

double VGOptions::get_double(const std::string& opt_name)
{
        return nx_options_get_double(m_opt.get(), opt_name.c_str());
}

std::string VGOptions::get_string(const std::string& opt_name)
{
        return nx_options_get_string(m_opt.get(), opt_name.c_str());
}

std::vector<std::string> VGOptions::get_rest()
{
        vector<string> r;
        char** rest = nx_options_get_rest(m_opt.get());

        if (rest != NULL) {
                int i = 0;
                while (rest[i] != NULL) {
                        string s = rest[i++];
                        r.push_back(s);
                }
        }

        return r;
}

bool VGOptions::is_set(const std::string& opt_name)
{
        return static_cast<bool>(nx_options_is_set(m_opt.get(), opt_name.c_str()));
}

void VGOptions::print_values(FILE* stream)
{
        nx_options_print_values(m_opt.get(), stream);
}

void VGOptions::print_usage(FILE* stream)
{
        nx_options_print_usage(m_opt.get(), stream);
}

void VGOptions::set_usage_header(const std::string& header)
{
        nx_options_set_usage_header(m_opt.get(), header.c_str());

}

void VGOptions::set_usage_footer(const std::string& footer)
{
        nx_options_set_usage_footer(m_opt.get(), footer.c_str());
}

}
}
