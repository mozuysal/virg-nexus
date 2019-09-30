/**
 * @file tests_json_lexer.cc
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
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <ctime>

#include "gtest/gtest.h"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_json_lexer.h"
#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_string.h"

using namespace std;

namespace {

class NXJSONLexerTest : public ::testing::Test {
protected:
        NXJSONLexerTest() {
        }

        virtual void SetUp() {
        }

        virtual void TearDown() {
        }
};

TEST_F(NXJSONLexerTest, new_free) {
        char *s = nx_strdup("{}");
        struct NXJSONLexer *jlex = nx_json_lexer_new(s);
        nx_json_lexer_free(jlex);
        nx_free(s);
}

TEST_F(NXJSONLexerTest, empty_object) {
        char *s = nx_strdup("{}");
        struct NXJSONLexer *jlex = nx_json_lexer_new(s);

        struct NXJSONToken t = { NX_JTT_INVALID, NULL};
        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_LCBRAC, t.type);
        EXPECT_EQ(NULL, t.text);
        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_RCBRAC, t.type);
        EXPECT_EQ(NULL, t.text);
        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_EOF, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_json_token_free_data(&t);
        nx_json_lexer_free(jlex);
        nx_free(s);
}

TEST_F(NXJSONLexerTest, empty_array) {
        char *s = nx_strdup("[]");
        struct NXJSONLexer *jlex = nx_json_lexer_new(s);

        struct NXJSONToken t = { NX_JTT_INVALID, NULL};
        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_LSBRAC, t.type);
        EXPECT_EQ(NULL, t.text);
        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_RSBRAC, t.type);
        EXPECT_EQ(NULL, t.text);
        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_EOF, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_json_token_free_data(&t);
        nx_json_lexer_free(jlex);
        nx_free(s);
}

TEST_F(NXJSONLexerTest, object) {
        char *s = nx_strdup("{ \"abc\" : 123 }");
        struct NXJSONLexer *jlex = nx_json_lexer_new(s);

        struct NXJSONToken t = { NX_JTT_INVALID, NULL};
        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_LCBRAC, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_STRING, t.type);
        EXPECT_EQ(0, strcmp(t.text, "abc"));
        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_COLON, t.type);
        EXPECT_EQ(NULL, t.text);
        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_INTEGER, t.type);
        EXPECT_EQ(0, strcmp(t.text, "123"));

        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_RCBRAC, t.type);
        EXPECT_EQ(NULL, t.text);
        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_EOF, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_json_token_free_data(&t);
        nx_json_lexer_free(jlex);
        nx_free(s);
}

TEST_F(NXJSONLexerTest, array) {
        char *s = nx_strdup("[ \"abc\", 123, 123.123, true, false, null ]");
        struct NXJSONLexer *jlex = nx_json_lexer_new(s);

        struct NXJSONToken t = { NX_JTT_INVALID, NULL};
        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_LSBRAC, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_STRING, t.type);
        EXPECT_EQ(0, strcmp(t.text, "abc"));
        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_COMMA, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_INTEGER, t.type);
        EXPECT_EQ(0, strcmp(t.text, "123"));
        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_COMMA, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_FPNUM, t.type);
        EXPECT_EQ(0, strcmp(t.text, "123.123"));
        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_COMMA, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_TRUE, t.type);
        EXPECT_EQ(NULL, t.text);
        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_COMMA, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_FALSE, t.type);
        EXPECT_EQ(NULL, t.text);
        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_COMMA, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_NULL, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_RSBRAC, t.type);
        EXPECT_EQ(NULL, t.text);
        nx_json_lexer_next_token(jlex, &t);
        EXPECT_EQ(NX_JTT_EOF, t.type);
        EXPECT_EQ(NULL, t.text);

        nx_json_token_free_data(&t);
        nx_json_lexer_free(jlex);
        nx_free(s);
}


} // namespace
