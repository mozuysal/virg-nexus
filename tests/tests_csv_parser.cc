/**
 * @file tests_csv_parser.cc
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2015 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <ctime>

#include "gtest/gtest.h"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_csv_parser.h"
#include "virg/nexus/nx_message.h"
#include "virg/nexus/nx_string.h"

using namespace std;

namespace {

class NXCSVParserTest : public ::testing::Test {
protected:
        NXCSVParserTest() {
        }

        virtual void SetUp() {
        }

        virtual void TearDown() {
        }
};

TEST_F(NXCSVParserTest, new_free) {
        char *s = nx_strdup("");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);
        struct NXCSVParser *cparse = nx_csv_parser_new(clex);

        nx_csv_parser_free(cparse);
        nx_csv_lexer_free(clex);
        nx_free(s);
}

TEST_F(NXCSVParserTest, parse_empty_DEATH) {
        char *s = nx_strdup("");
        struct NXCSVLexer *clex = nx_csv_lexer_new(s);
        struct NXCSVParser *cparse = nx_csv_parser_new(clex);

        EXPECT_DEATH({nx_csv_parser_parse(cparse);},"Can not parse empty CSV!");
}

} // namespace
