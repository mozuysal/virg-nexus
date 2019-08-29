/**
 * @file vg_options.cc
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
#include "virg/nexus/vg_options.hpp"

using std::string;
using std::vector;

namespace virg {
namespace nexus {

VGOptions::VGOptions()
{
        m_opt = nx_options_alloc();
}

VGOptions::~VGOptions()
{
        if (m_opt)
                nx_options_free(m_opt);
}

void VGOptions::add_bool(const std::string& switches, const std::string& help_text,  enum VGOptions::RequirementState is_required, bool defaut_value)
{
        const string opt_format = (is_required == VGOptions::REQUIRED) ? "B" : "b";
        NXBool def_val = static_cast<NXBool>(defaut_value);
        nx_options_add(m_opt, opt_format.c_str(), switches.c_str(), help_text.c_str(), def_val);
}

void VGOptions::add_int(const std::string& switches, const std::string& help_text,  enum VGOptions::RequirementState is_required, int defaut_value)
{
        const string opt_format = (is_required == VGOptions::REQUIRED) ? "I" : "i";
        nx_options_add(m_opt, opt_format.c_str(), switches.c_str(), help_text.c_str(), defaut_value);
}

void VGOptions::add_double(const std::string& switches, const std::string& help_text,  enum VGOptions::RequirementState is_required, double defaut_value)
{
        const string opt_format = (is_required == VGOptions::REQUIRED) ? "D" : "d";
        nx_options_add(m_opt, opt_format.c_str(), switches.c_str(), help_text.c_str(), defaut_value);
}

void VGOptions::add_string(const std::string& switches, const std::string& help_text,  enum VGOptions::RequirementState is_required, const std::string& defaut_value)
{
        const string opt_format = (is_required == VGOptions::REQUIRED) ? "S" : "s";
        nx_options_add(m_opt, opt_format.c_str(), switches.c_str(), help_text.c_str(), defaut_value.c_str());
}

void VGOptions::add_rest(const std::string& help_text,  enum VGOptions::RequirementState is_required)
{
        const string opt_format = (is_required == VGOptions::REQUIRED) ? "R" : "r";
        nx_options_add(m_opt, opt_format.c_str(), help_text.c_str());
}

void VGOptions::add_help()
{
        nx_options_add_help(m_opt);
}

void VGOptions::set_from_args(int argc, char **argv)
{
        nx_options_set_from_args(m_opt, argc, argv);
}

bool VGOptions::get_bool(const std::string& opt_name)
{
        NXBool r = nx_options_get_bool(m_opt, opt_name.c_str());
        return static_cast<bool>(r);
}

int VGOptions::get_int(const std::string& opt_name)
{
        return nx_options_get_int(m_opt, opt_name.c_str());
}

double VGOptions::get_double(const std::string& opt_name)
{
        return nx_options_get_double(m_opt, opt_name.c_str());
}

std::string VGOptions::get_string(const std::string& opt_name)
{
        return nx_options_get_string(m_opt, opt_name.c_str());
}

std::vector<std::string> VGOptions::get_rest()
{
        vector<string> r;
        char** rest = nx_options_get_rest(m_opt);

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
        return static_cast<bool>(nx_options_is_set(m_opt, opt_name.c_str()));
}

void VGOptions::print_values(FILE* stream)
{
        nx_options_print_values(m_opt, stream);
}

void VGOptions::print_usage(FILE* stream)
{
        nx_options_print_usage(m_opt, stream);
}

void VGOptions::set_usage_header(const std::string& header)
{
        nx_options_set_usage_header(m_opt, header.c_str());

}

void VGOptions::set_usage_footer(const std::string& footer)
{
        nx_options_set_usage_footer(m_opt, footer.c_str());
}

}
}
