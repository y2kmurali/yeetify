#include "src/yeet_generator.h"

#include <gtest/gtest.h>
#include <regex>
#include <string>
#include <unordered_set>
#include <vector>

namespace yeetify {
namespace {

TEST(YeetGeneratorTest, GeneratesZeroYeets) {
    YeetGenerator generator(42);
    auto yeets = generator.Generate(0);
    EXPECT_TRUE(yeets.empty());
}

TEST(YeetGeneratorTest, GeneratesSingleYeetWithoutCrashing) {
    YeetGenerator generator(42);
    auto yeets = generator.Generate(1);
    ASSERT_EQ(yeets.size(), 1u);
    EXPECT_TRUE(std::regex_match(yeets[0], std::regex("[yY][eE]+[tT]+")));
}

TEST(YeetGeneratorTest, GeneratesRequestedCountAndAllUnique) {
    YeetGenerator generator(12345);
    const size_t count = 500;
    auto yeets = generator.Generate(count);
    ASSERT_EQ(yeets.size(), count);

    std::unordered_set<std::string> unique_yeets(yeets.begin(), yeets.end());
    EXPECT_EQ(unique_yeets.size(), count);
}

TEST(YeetGeneratorTest, AllYeetsMatchValidIdentifierPattern) {
    YeetGenerator generator(999);
    auto yeets = generator.Generate(100);

    std::regex yeet_regex("^[yY][eE]+[tT]+[0-9]*$");
    for (const auto& y : yeets) {
        EXPECT_TRUE(std::regex_match(y, yeet_regex))
            << "Yeet '" << y << "' does not match identifier pattern";
    }
}

TEST(YeetGeneratorTest, DeterministicWithSameSeed) {
    YeetGenerator gen1(42);
    YeetGenerator gen2(42);

    auto list1 = gen1.Generate(50);
    auto list2 = gen2.Generate(50);

    EXPECT_EQ(list1, list2);
}

TEST(YeetGeneratorTest, DifferentWithDifferentSeed) {
    YeetGenerator gen1(1);
    YeetGenerator gen2(2);

    auto list1 = gen1.Generate(50);
    auto list2 = gen2.Generate(50);

    EXPECT_NE(list1, list2);
}

}  // namespace
}  // namespace yeetify
