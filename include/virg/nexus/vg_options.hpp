/**
 * @file vg_options.hpp
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
#ifndef VIRG_NEXUS_VG_OPTIONS_HPP
#define VIRG_NEXUS_VG_OPTIONS_HPP

#include <string>
#include <vector>

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
        struct NXOptions* m_opt;
};

}
}

#endif
