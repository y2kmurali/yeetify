#pragma once

#include <cstddef>
#include <random>
#include <string>
#include <vector>

namespace yeetify {

class YeetGenerator {
public:
    // If seed is provided, uses that seed. Otherwise uses random_device.
    explicit YeetGenerator(unsigned int seed = 1337);

    // Generates `count` unique yeet permutations.
    std::vector<std::string> Generate(size_t count);

private:
    std::mt19937 rng_;
};

}  // namespace yeetify
