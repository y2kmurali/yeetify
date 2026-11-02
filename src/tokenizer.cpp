#include "src/tokenizer.h"

#include <clang-c/Index.h>
#include <sstream>
#include <unordered_set>
#include <vector>

namespace yeetify {

namespace {

// Helper to find which line numbers in the source code are preprocessor directive lines.
std::unordered_set<unsigned int> FindDirectiveLines(const std::string& source_code) {
    std::unordered_set<unsigned int> directive_lines;
    std::istringstream stream(source_code);
    std::string line;
    unsigned int current_line = 1;
    bool in_multiline_directive = false;

    while (std::getline(stream, line)) {
        // Strip trailing \r if present
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        // Find first non-whitespace character
        size_t first_non_space = line.find_first_not_of(" \t");

        if (in_multiline_directive) {
            directive_lines.insert(current_line);
            // Check if line continuation continues
            if (!line.empty() && line.back() == '\\') {
                in_multiline_directive = true;
            } else {
                in_multiline_directive = false;
            }
        } else if (first_non_space != std::string::npos && line[first_non_space] == '#') {
            directive_lines.insert(current_line);
            if (!line.empty() && line.back() == '\\') {
                in_multiline_directive = true;
            }
        }

        current_line++;
    }

    return directive_lines;
}

}  // namespace

std::vector<Token> Tokenizer::Tokenize(const std::string& source_code,
                                       const std::string& filename) {
    std::unordered_set<unsigned int> directive_lines = FindDirectiveLines(source_code);

    CXIndex index = clang_createIndex(0, 0);

    CXUnsavedFile unsaved_file;
    unsaved_file.Filename = filename.c_str();
    unsaved_file.Contents = source_code.c_str();
    unsaved_file.Length = source_code.size();

    const char* clang_args[] = {
        "-std=c++17",
        "-x",
        "c++",
        "-Wno-everything",
    };
    int num_args = sizeof(clang_args) / sizeof(clang_args[0]);

    unsigned options = CXTranslationUnit_DetailedPreprocessingRecord |
                       CXTranslationUnit_KeepGoing;

    CXTranslationUnit tu = clang_parseTranslationUnit(
        index, filename.c_str(), clang_args, num_args, &unsaved_file, 1, options);

    if (!tu) {
        clang_disposeIndex(index);
        return {};
    }

    CXCursor cursor = clang_getTranslationUnitCursor(tu);
    CXSourceRange range = clang_getCursorExtent(cursor);
    CXToken* clang_tokens = nullptr;
    unsigned num_tokens = 0;
    clang_tokenize(tu, range, &clang_tokens, &num_tokens);

    std::vector<Token> result;
    result.reserve(num_tokens);

    for (unsigned i = 0; i < num_tokens; ++i) {
        CXToken clang_tok = clang_tokens[i];
        CXTokenKind kind = clang_getTokenKind(clang_tok);

        TokenType type;
        switch (kind) {
            case CXToken_Punctuation:
                type = TokenType::Punctuation;
                break;
            case CXToken_Keyword:
                type = TokenType::Keyword;
                break;
            case CXToken_Identifier:
                type = TokenType::Identifier;
                break;
            case CXToken_Literal:
                type = TokenType::Literal;
                break;
            case CXToken_Comment:
                type = TokenType::Comment;
                break;
            default:
                type = TokenType::Other;
                break;
        }

        CXString spelling = clang_getTokenSpelling(tu, clang_tok);
        const char* c_str = clang_getCString(spelling);
        std::string text = c_str ? c_str : "";
        clang_disposeString(spelling);

        CXSourceLocation loc = clang_getTokenLocation(tu, clang_tok);
        unsigned line = 0, column = 0, offset = 0;
        clang_getSpellingLocation(loc, nullptr, &line, &column, &offset);

        bool is_in_directive = directive_lines.find(line) != directive_lines.end();

        result.push_back(Token{
            type,
            std::move(text),
            line,
            column,
            offset,
            is_in_directive,
        });
    }

    clang_disposeTokens(tu, clang_tokens, num_tokens);
    clang_disposeTranslationUnit(tu);
    clang_disposeIndex(index);

    return result;
}

}  // namespace yeetify
