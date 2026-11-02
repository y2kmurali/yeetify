#include "src/tokenizer.h"

#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace yeetify {
namespace {

TEST(TokenizerTest, TokenizesSimpleFunction) {
    Tokenizer tokenizer;
    std::string source = "int main() {\n    return 0;\n}";
    std::vector<Token> tokens = tokenizer.Tokenize(source);

    std::vector<std::string> expected_texts = {
        "int", "main", "(", ")", "{", "return", "0", ";", "}"
    };

    ASSERT_EQ(tokens.size(), expected_texts.size());
    for (size_t i = 0; i < expected_texts.size(); ++i) {
        EXPECT_EQ(tokens[i].text, expected_texts[i]);
    }

    EXPECT_EQ(tokens[0].type, TokenType::Keyword);
    EXPECT_EQ(tokens[1].type, TokenType::Identifier);
    EXPECT_EQ(tokens[2].type, TokenType::Punctuation);
    EXPECT_EQ(tokens[3].type, TokenType::Punctuation);
    EXPECT_EQ(tokens[4].type, TokenType::Punctuation);
    EXPECT_EQ(tokens[5].type, TokenType::Keyword);
    EXPECT_EQ(tokens[6].type, TokenType::Literal);
    EXPECT_EQ(tokens[7].type, TokenType::Punctuation);
    EXPECT_EQ(tokens[8].type, TokenType::Punctuation);
}

TEST(TokenizerTest, HandlesStringLiteralsWithSpaces) {
    Tokenizer tokenizer;
    std::string source = "std::cout << \"Hello, World!\\n\";";
    std::vector<Token> tokens = tokenizer.Tokenize(source);

    std::vector<std::string> expected_texts = {
        "std", "::", "cout", "<<", "\"Hello, World!\\n\"", ";"
    };

    ASSERT_EQ(tokens.size(), expected_texts.size());
    for (size_t i = 0; i < expected_texts.size(); ++i) {
        EXPECT_EQ(tokens[i].text, expected_texts[i]);
    }
    EXPECT_EQ(tokens[4].type, TokenType::Literal);
}

TEST(TokenizerTest, HandlesOperatorsWithoutSpaces) {
    Tokenizer tokenizer;
    std::string source = "a+=b*c-(d/e);";
    std::vector<Token> tokens = tokenizer.Tokenize(source);

    std::vector<std::string> expected_texts = {
        "a", "+=", "b", "*", "c", "-", "(", "d", "/", "e", ")", ";"
    };

    ASSERT_EQ(tokens.size(), expected_texts.size());
    for (size_t i = 0; i < expected_texts.size(); ++i) {
        EXPECT_EQ(tokens[i].text, expected_texts[i]);
    }
}

TEST(TokenizerTest, IdentifiesDirectiveLines) {
    Tokenizer tokenizer;
    std::string source =
        "#include <iostream>\n"
        "#define FOO 1\n"
        "int x = FOO;\n";
    std::vector<Token> tokens = tokenizer.Tokenize(source);

    // Tokens on lines 1 and 2 should have is_in_directive == true
    for (const auto& token : tokens) {
        if (token.line == 1 || token.line == 2) {
            EXPECT_TRUE(token.is_in_directive)
                << "Token '" << token.text << "' on line " << token.line
                << " should be marked as is_in_directive";
        } else {
            EXPECT_FALSE(token.is_in_directive)
                << "Token '" << token.text << "' on line " << token.line
                << " should NOT be marked as is_in_directive";
        }
    }
}

TEST(TokenizerTest, HandlesComments) {
    Tokenizer tokenizer;
    std::string source = "// This is a comment\nint x = 5; /* inline comment */";
    std::vector<Token> tokens = tokenizer.Tokenize(source);

    EXPECT_EQ(tokens[0].type, TokenType::Comment);
    EXPECT_EQ(tokens[0].text, "// This is a comment");
    EXPECT_EQ(tokens[1].text, "int");
    EXPECT_EQ(tokens[2].text, "x");
    EXPECT_EQ(tokens[3].text, "=");
    EXPECT_EQ(tokens[4].text, "5");
    EXPECT_EQ(tokens[5].text, ";");
    EXPECT_EQ(tokens[6].type, TokenType::Comment);
    EXPECT_EQ(tokens[6].text, "/* inline comment */");
}

TEST(TokenizerTest, HandlesCharacterLiterals) {
    Tokenizer tokenizer;
    std::string source = "char c = '\\n';";
    std::vector<Token> tokens = tokenizer.Tokenize(source);

    EXPECT_EQ(tokens[0].text, "char");
    EXPECT_EQ(tokens[1].text, "c");
    EXPECT_EQ(tokens[2].text, "=");
    EXPECT_EQ(tokens[3].text, "'\\n'");
    EXPECT_EQ(tokens[3].type, TokenType::Literal);
    EXPECT_EQ(tokens[4].text, ";");
}

}  // namespace
}  // namespace yeetify
