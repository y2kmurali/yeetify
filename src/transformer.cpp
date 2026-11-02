#include "src/transformer.h"

#include <sstream>
#include <unordered_set>
#include <vector>

namespace yeetify {

Transformer::Transformer(unsigned int seed)
    : generator_(seed), tokenizer_() {}

TransformResult Transformer::Transform(const std::string& source_code,
                                       const std::string& filename) {
    TransformResult result;
    if (source_code.empty()) {
        return result;
    }

    std::vector<Token> tokens = tokenizer_.Tokenize(source_code, filename);
    if (tokens.empty()) {
        return result;
    }

    // Split source code into lines to preserve preprocessor directives exactly
    std::istringstream stream(source_code);
    std::string line_str;
    std::vector<std::string> all_lines;
    while (std::getline(stream, line_str)) {
        if (!line_str.empty() && line_str.back() == '\r') {
            line_str.pop_back();
        }
        all_lines.push_back(line_str);
    }

    // Identify directive lines
    std::unordered_set<unsigned int> directive_line_nums;
    for (const auto& token : tokens) {
        if (token.is_in_directive) {
            directive_line_nums.insert(token.line);
        }
    }

    for (unsigned int line_num : directive_line_nums) {
        if (line_num >= 1 && line_num <= all_lines.size()) {
            result.directive_lines.push_back(all_lines[line_num - 1]);
        }
    }

    // Collect distinct tokens to replace in order of appearance
    std::vector<std::string> unique_tokens;
    std::unordered_set<std::string> seen_tokens;

    for (const auto& token : tokens) {
        if (token.is_in_directive || token.type == TokenType::Comment) {
            continue;
        }
        if (seen_tokens.insert(token.text).second) {
            unique_tokens.push_back(token.text);
        }
    }

    // Generate unique yeet identifiers
    std::vector<std::string> yeets = generator_.Generate(unique_tokens.size());
    for (size_t i = 0; i < unique_tokens.size(); ++i) {
        result.token_to_yeet[unique_tokens[i]] = yeets[i];
        result.yeet_to_token[yeets[i]] = unique_tokens[i];
    }

    // Construct the output
    std::ostringstream out;

    // 1. Directives first
    for (const auto& dir : result.directive_lines) {
        out << dir << "\n";
    }
    if (!result.directive_lines.empty()) {
        out << "\n";
    }

    // 2. Macro defines
    for (size_t i = 0; i < unique_tokens.size(); ++i) {
        out << "#define " << yeets[i] << " " << unique_tokens[i] << "\n";
    }
    if (!unique_tokens.empty()) {
        out << "\n";
    }

    // 3. Body
    unsigned int current_line = 0;
    for (const auto& token : tokens) {
        if (token.is_in_directive) {
            continue;
        }

        if (current_line == 0) {
            current_line = token.line;
        } else if (token.line > current_line) {
            while (current_line < token.line) {
                out << "\n";
                current_line++;
            }
        } else {
            out << " ";
        }

        if (token.type == TokenType::Comment) {
            out << token.text;
        } else {
            auto it = result.token_to_yeet.find(token.text);
            if (it != result.token_to_yeet.end()) {
                out << it->second;
            } else {
                out << token.text;
            }
        }
    }

    out << "\n";
    result.yeetified_code = out.str();
    return result;
}

}  // namespace yeetify
