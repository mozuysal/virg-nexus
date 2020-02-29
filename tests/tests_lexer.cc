/**
 * @file tests_lexer.cc
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
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <ctime>

#include "gtest/gtest.h"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_lexer.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_string.h"

using namespace std;

namespace {

class NXLexerTest : public ::testing::Test {
protected:
        NXLexerTest() {
        }

        virtual void SetUp() {
                str_empty = nx_strdup("");
        }

        virtual void TearDown() {
                nx_free(str_empty);
        }

        char *str_empty;
};

TEST_F(NXLexerTest, new_free) {
        struct NXLexer *lex = nx_lexer_new(str_empty);
        EXPECT_TRUE(nx_lexer_text(lex) == str_empty);
        EXPECT_EQ(EOF, nx_lexer_current_char(lex));
        EXPECT_EQ(0, nx_lexer_position(lex));
        EXPECT_EQ(1, nx_lexer_line_no(lex));
        EXPECT_EQ(1, nx_lexer_column_no(lex));
        nx_lexer_free(lex);
}

TEST_F(NXLexerTest, match) {
        char *str = nx_strdup("ABCDEFG\nHIJK");
        struct NXLexer *lex = nx_lexer_new(str);

        EXPECT_TRUE(nx_lexer_match(lex, "ABCD"));
        EXPECT_EQ('E', nx_lexer_current_char(lex));
        EXPECT_EQ(4, nx_lexer_position(lex));
        EXPECT_EQ(1, nx_lexer_line_no(lex));
        EXPECT_EQ(5, nx_lexer_column_no(lex));

        EXPECT_FALSE(nx_lexer_match(lex, "EFGG"));
        EXPECT_EQ('E', nx_lexer_current_char(lex));
        EXPECT_EQ(4, nx_lexer_position(lex));
        EXPECT_EQ(1, nx_lexer_line_no(lex));
        EXPECT_EQ(5, nx_lexer_column_no(lex));

        EXPECT_TRUE(nx_lexer_match(lex, "EFG\nH"));
        EXPECT_EQ('I', nx_lexer_current_char(lex));
        EXPECT_EQ(9, nx_lexer_position(lex));
        EXPECT_EQ(2, nx_lexer_line_no(lex));
        EXPECT_EQ(2, nx_lexer_column_no(lex));

        EXPECT_FALSE(nx_lexer_match(lex, "IJKKKKK"));
        EXPECT_EQ('I', nx_lexer_current_char(lex));
        EXPECT_EQ(9, nx_lexer_position(lex));
        EXPECT_EQ(2, nx_lexer_line_no(lex));
        EXPECT_EQ(2, nx_lexer_column_no(lex));

        EXPECT_TRUE(nx_lexer_match(lex, "IJK"));
        EXPECT_EQ(EOF, nx_lexer_current_char(lex));
        EXPECT_EQ(12, nx_lexer_position(lex));
        EXPECT_EQ(2, nx_lexer_line_no(lex));
        EXPECT_EQ(5, nx_lexer_column_no(lex));

        EXPECT_FALSE(nx_lexer_match(lex, "IJKKKKK"));
        EXPECT_EQ(EOF, nx_lexer_current_char(lex));
        EXPECT_EQ(12, nx_lexer_position(lex));
        EXPECT_EQ(2, nx_lexer_line_no(lex));
        EXPECT_EQ(5, nx_lexer_column_no(lex));

        nx_lexer_free(lex);
        nx_free(str);
}

TEST_F(NXLexerTest, numbers) {
        char *str = nx_strdup("0 0.0 1 1.0 1.00 -1 -1.0 -1.00 0.1 -0.1 1e0 1E0 1.0e0");
        struct NXLexer *lex = nx_lexer_new(str);

        int len;
        enum NXLexerNumberType type;
        const char *res;

        res = nx_lexer_NUMBER(lex, &len, &type);
        EXPECT_EQ(1, len);
        EXPECT_EQ(NX_LNT_INTEGER, type);
        EXPECT_EQ(0, strncmp(res, "0", len));
        nx_lexer_consume(lex);

        res = nx_lexer_NUMBER(lex, &len, &type);
        EXPECT_EQ(3, len);
        EXPECT_EQ(NX_LNT_FLOATING_POINT, type);
        EXPECT_EQ(0, strncmp(res, "0.0", len));
        nx_lexer_consume(lex);

        res = nx_lexer_NUMBER(lex, &len, &type);
        EXPECT_EQ(1, len);
        EXPECT_EQ(NX_LNT_INTEGER, type);
        EXPECT_EQ(0, strncmp(res, "1", len));
        nx_lexer_consume(lex);

        res = nx_lexer_NUMBER(lex, &len, &type);
        EXPECT_EQ(3, len);
        EXPECT_EQ(NX_LNT_FLOATING_POINT, type);
        EXPECT_EQ(0, strncmp(res, "1.0", len));
        nx_lexer_consume(lex);

        res = nx_lexer_NUMBER(lex, &len, &type);
        EXPECT_EQ(4, len);
        EXPECT_EQ(NX_LNT_FLOATING_POINT, type);
        EXPECT_EQ(0, strncmp(res, "1.00", len));
        nx_lexer_consume(lex);

        res = nx_lexer_NUMBER(lex, &len, &type);
        EXPECT_EQ(2, len);
        EXPECT_EQ(NX_LNT_INTEGER, type);
        EXPECT_EQ(0, strncmp(res, "-1", len));
        nx_lexer_consume(lex);

        res = nx_lexer_NUMBER(lex, &len, &type);
        EXPECT_EQ(4, len);
        EXPECT_EQ(NX_LNT_FLOATING_POINT, type);
        EXPECT_EQ(0, strncmp(res, "-1.0", len));
        nx_lexer_consume(lex);

        res = nx_lexer_NUMBER(lex, &len, &type);
        EXPECT_EQ(5, len);
        EXPECT_EQ(NX_LNT_FLOATING_POINT, type);
        EXPECT_EQ(0, strncmp(res, "-1.00", len));
        nx_lexer_consume(lex);

        res = nx_lexer_NUMBER(lex, &len, &type);
        EXPECT_EQ(3, len);
        EXPECT_EQ(NX_LNT_FLOATING_POINT, type);
        EXPECT_EQ(0, strncmp(res, "0.1", len));
        nx_lexer_consume(lex);

        res = nx_lexer_NUMBER(lex, &len, &type);
        EXPECT_EQ(4, len);
        EXPECT_EQ(NX_LNT_FLOATING_POINT, type);
        EXPECT_EQ(0, strncmp(res, "-0.1", len));
        nx_lexer_consume(lex);

        res = nx_lexer_NUMBER(lex, &len, &type);
        EXPECT_EQ(3, len);
        EXPECT_EQ(NX_LNT_FLOATING_POINT, type);
        EXPECT_EQ(0, strncmp(res, "1e0", len));
        nx_lexer_consume(lex);

        res = nx_lexer_NUMBER(lex, &len, &type);
        EXPECT_EQ(3, len);
        EXPECT_EQ(NX_LNT_FLOATING_POINT, type);
        EXPECT_EQ(0, strncmp(res, "1E0", len));
        nx_lexer_consume(lex);

        res = nx_lexer_NUMBER(lex, &len, &type);
        EXPECT_EQ(5, len);
        EXPECT_EQ(NX_LNT_FLOATING_POINT, type);
        EXPECT_EQ(0, strncmp(res, "1.0e0", len));
        nx_lexer_consume(lex);

        nx_lexer_free(lex);
        nx_free(str);
}

TEST_F(NXLexerTest, quoted_strings) {
        char *str = nx_strdup("\"\" \"a\" \"\\\"\" \"\\\\\" \"\\n\" \"abcd\" \"123\"");
        struct NXLexer *lex = nx_lexer_new(str);

        int len;
        const char *res;

        res = nx_lexer_QUOTED_STRING(lex, &len);
        EXPECT_EQ(0, len);
        EXPECT_EQ(0, strncmp(res, "", len));
        EXPECT_EQ(2, nx_lexer_position(lex));
        nx_lexer_consume(lex);

        res = nx_lexer_QUOTED_STRING(lex, &len);
        EXPECT_EQ(1, len);
        EXPECT_EQ(0, strncmp(res, "a", len));
        nx_lexer_consume(lex);

        res = nx_lexer_QUOTED_STRING(lex, &len);
        EXPECT_EQ(2, len);
        EXPECT_EQ(0, strncmp(res, "\\\"", len));
        nx_lexer_consume(lex);

        res = nx_lexer_QUOTED_STRING(lex, &len);
        EXPECT_EQ(2, len);
        EXPECT_EQ(0, strncmp(res, "\\\\", len));
        nx_lexer_consume(lex);

        res = nx_lexer_QUOTED_STRING(lex, &len);
        EXPECT_EQ(2, len);
        EXPECT_EQ(0, strncmp(res, "\\n", len));
        nx_lexer_consume(lex);

        res = nx_lexer_QUOTED_STRING(lex, &len);
        EXPECT_EQ(4, len);
        EXPECT_EQ(0, strncmp(res, "abcd", len));
        nx_lexer_consume(lex);

        res = nx_lexer_QUOTED_STRING(lex, &len);
        EXPECT_EQ(3, len);
        EXPECT_EQ(0, strncmp(res, "123", len));
        nx_lexer_consume(lex);

        nx_lexer_free(lex);
        nx_free(str);
}

} // namespace
