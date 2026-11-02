#include "src/yeet_generator.h"

#include <unordered_set>

namespace yeetify {

YeetGenerator::YeetGenerator(unsigned int seed) : rng_(seed) {}

std::vector<std::string> YeetGenerator::Generate(size_t count) {
    std::vector<std::string> result;
    if (count == 0) {
        return result;
    }

    result.reserve(count);
    std::unordered_set<std::string> seen;
    seen.reserve(count);

    size_t max_e = 3;
    size_t max_t = 3;
    size_t consecutive_collisions = 0;

    std::uniform_int_distribution<int> bool_dist(0, 1);

    while (result.size() < count) {
        std::uniform_int_distribution<size_t> e_dist(1, max_e);
        std::uniform_int_distribution<size_t> t_dist(1, max_t);

        size_t e_count = e_dist(rng_);
        size_t t_count = t_dist(rng_);

        std::string s;
        s.reserve(1 + e_count + t_count);

        // Leading y or Y
        s.push_back(bool_dist(rng_) ? 'y' : 'Y');

        for (size_t i = 0; i < e_count; ++i) {
            s.push_back(bool_dist(rng_) ? 'e' : 'E');
        }

        for (size_t i = 0; i < t_count; ++i) {
            s.push_back(bool_dist(rng_) ? 't' : 'T');
        }

        if (seen.insert(s).second) {
            result.push_back(s);
            consecutive_collisions = 0;
        } else {
            consecutive_collisions++;
            if (consecutive_collisions > 20) {
                // Expand space of e and t to avoid pigeonhole saturation
                max_e++;
                max_t++;
                consecutive_collisions = 0;
            }
        }
    }

    return result;
}

}  // namespace yeetify
