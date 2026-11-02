#pragma once

#include <string>
#include <vector>

namespace yeetify {

enum class TokenType {
    Punctuation,
    Keyword,
    Identifier,
    Literal,
    Comment,
    Other,
};

struct Token {
    TokenType type;
    std::string text;
    unsigned int line;
    unsigned int column;
    unsigned int offset;
    bool is_in_directive = false;
};

struct TokenizeResult {
    std::vector<Token> tokens;
    // Lines that are preprocessor directives, preserved in order
    std::vector<std::string> directive_lines;
};

class Tokenizer {
public:
    Tokenizer() = default;
    ~Tokenizer() = default;

    // Tokenizes a C/C++ source code string.
    // filename_for_clang is a dummy or actual file name (e.g. "input.cpp") to inform clang of language standard.
    std::vector<Token> Tokenize(const std::string& source_code,
                                const std::string& filename = "input.cpp");
};

}  // namespace yeetify
