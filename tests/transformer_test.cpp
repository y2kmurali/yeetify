#include "src/transformer.h"

#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace yeetify {
namespace {

TEST(TransformerTest, HandlesEmptyCode) {
    Transformer transformer(42);
    auto result = transformer.Transform("");
    EXPECT_TRUE(result.token_to_yeet.empty());
    EXPECT_TRUE(result.yeetified_code.empty());
}

TEST(TransformerTest, PreservesDirectivesAtTop) {
    Transformer transformer(42);
    std::string source =
        "#include <iostream>\n"
        "#include <vector>\n"
        "int main() { return 0; }";

    auto result = transformer.Transform(source);

    EXPECT_NE(result.yeetified_code.find("#include <iostream>"), std::string::npos);
    EXPECT_NE(result.yeetified_code.find("#include <vector>"), std::string::npos);

    // Directives must appear before #define statements
    size_t inc_pos = result.yeetified_code.find("#include <vector>");
    size_t def_pos = result.yeetified_code.find("#define ");
    EXPECT_LT(inc_pos, def_pos);
}

TEST(TransformerTest, HandlesStringLiteralsWithSpacesInDefines) {
    Transformer transformer(42);
    std::string source =
        "#include <iostream>\n"
        "int main() {\n"
        "    std::cout << \"Hello, World! Yeet count: 123\" << std::endl;\n"
        "    return 0;\n"
        "}";

    auto result = transformer.Transform(source);

    // The whole string literal must be in token_to_yeet
    std::string string_lit = "\"Hello, World! Yeet count: 123\"";
    ASSERT_TRUE(result.token_to_yeet.find(string_lit) != result.token_to_yeet.end());

    std::string yeet_for_string = result.token_to_yeet[string_lit];
    std::string expected_define = "#define " + yeet_for_string + " " + string_lit;
    EXPECT_NE(result.yeetified_code.find(expected_define), std::string::npos);
}

TEST(TransformerTest, MapsEachCodeTokenToYeet) {
    Transformer transformer(42);
    std::string source = "int x = 42;";
    auto result = transformer.Transform(source);

    EXPECT_TRUE(result.token_to_yeet.find("int") != result.token_to_yeet.end());
    EXPECT_TRUE(result.token_to_yeet.find("x") != result.token_to_yeet.end());
    EXPECT_TRUE(result.token_to_yeet.find("=") != result.token_to_yeet.end());
    EXPECT_TRUE(result.token_to_yeet.find("42") != result.token_to_yeet.end());
    EXPECT_TRUE(result.token_to_yeet.find(";") != result.token_to_yeet.end());
}

}  // namespace
}  // namespace yeetify
