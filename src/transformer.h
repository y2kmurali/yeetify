#pragma once

#include "src/tokenizer.h"
#include "src/yeet_generator.h"

#include <map>
#include <string>
#include <vector>

namespace yeetify {

struct TransformResult {
    std::string yeetified_code;
    std::vector<std::string> directive_lines;
    std::map<std::string, std::string> token_to_yeet;  // original token -> yeet identifier
    std::map<std::string, std::string> yeet_to_token;  // yeet identifier -> original token
};

class Transformer {
public:
    explicit Transformer(unsigned int seed = 1337);

    // Transforms source code into yeetified C/C++ code.
    TransformResult Transform(const std::string& source_code,
                              const std::string& filename = "input.cpp");

private:
    YeetGenerator generator_;
    Tokenizer tokenizer_;
};

}  // namespace yeetify
