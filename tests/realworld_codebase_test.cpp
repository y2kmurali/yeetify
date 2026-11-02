#include "src/transformer.h"

#include <gtest/gtest.h>
#include <array>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

namespace yeetify {
namespace {

std::string ReadFileToString(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<uint8_t> ReadBinaryFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return {};
    }
    return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());
}

std::string RunCommand(const std::string& command, int& exit_code) {
    std::array<char, 256> buffer;
    std::string result;
    FILE* pipe = popen((command + " 2>&1").c_str(), "r");
    if (!pipe) {
        exit_code = -1;
        return "popen failed";
    }
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data();
    }
    exit_code = pclose(pipe);
    return result;
}

TEST(RealworldCodebaseTest, TinyRaytracerRunsIdentically) {
    std::string raytracer_source = ReadFileToString("tests/testdata/tinyraytracer.cpp");
    ASSERT_FALSE(raytracer_source.empty()) << "Could not read tinyraytracer.cpp";

    Transformer transformer(2026);
    TransformResult result = transformer.Transform(raytracer_source, "tinyraytracer.cpp");
    ASSERT_FALSE(result.yeetified_code.empty());
    EXPECT_GT(result.token_to_yeet.size(), 100u)
        << "Expected over 100 distinct tokens in raytracer to be yeetified";

    // Create temporary directory for isolated execution
    char temp_dir[] = "/tmp/yeet_ray_XXXXXX";
    ASSERT_NE(mkdtemp(temp_dir), nullptr);

    std::string orig_cpp = std::string(temp_dir) + "/orig.cpp";
    std::string yeet_cpp = std::string(temp_dir) + "/yeet.cpp";
    std::string orig_bin = std::string(temp_dir) + "/orig_bin";
    std::string yeet_bin = std::string(temp_dir) + "/yeet_bin";

    // Write sources
    {
        std::ofstream f(orig_cpp);
        f << raytracer_source;
    }
    {
        std::ofstream f(yeet_cpp);
        f << result.yeetified_code;
    }

    // Compile original
    int orig_compile_rc = 0;
    std::string orig_compile_out = RunCommand("clang++ -std=c++17 -O3 " + orig_cpp + " -o " + orig_bin, orig_compile_rc);
    ASSERT_EQ(orig_compile_rc, 0) << "Compilation of original failed:\n" << orig_compile_out;

    // Compile yeetified
    int yeet_compile_rc = 0;
    std::string yeet_compile_out = RunCommand("clang++ -std=c++17 -O3 " + yeet_cpp + " -o " + yeet_bin, yeet_compile_rc);
    ASSERT_EQ(yeet_compile_rc, 0) << "Compilation of yeetified failed:\n" << yeet_compile_out;

    // Run original in temp dir
    int orig_run_rc = 0;
    std::string orig_run_out = RunCommand("cd " + std::string(temp_dir) + " && ./orig_bin", orig_run_rc);
    ASSERT_EQ(orig_run_rc, 0) << "Original run failed:\n" << orig_run_out;

    std::string orig_ppm = std::string(temp_dir) + "/out.ppm";
    std::string orig_ppm_saved = std::string(temp_dir) + "/out_orig.ppm";
    rename(orig_ppm.c_str(), orig_ppm_saved.c_str());

    // Run yeetified in temp dir
    int yeet_run_rc = 0;
    std::string yeet_run_out = RunCommand("cd " + std::string(temp_dir) + " && ./yeet_bin", yeet_run_rc);
    ASSERT_EQ(yeet_run_rc, 0) << "Yeetified run failed:\n" << yeet_run_out;

    std::string yeet_ppm = std::string(temp_dir) + "/out.ppm";

    // Compare rendered image files byte-for-byte
    std::vector<uint8_t> orig_bytes = ReadBinaryFile(orig_ppm_saved);
    std::vector<uint8_t> yeet_bytes = ReadBinaryFile(yeet_ppm);

    ASSERT_FALSE(orig_bytes.empty());
    ASSERT_FALSE(yeet_bytes.empty());
    EXPECT_EQ(orig_bytes.size(), yeet_bytes.size());
    EXPECT_EQ(orig_bytes, yeet_bytes)
        << "The raytraced image rendered by the yeetified binary does not match the original!";

    // Clean up
    unlink(orig_cpp.c_str());
    unlink(yeet_cpp.c_str());
    unlink(orig_bin.c_str());
    unlink(yeet_bin.c_str());
    unlink(orig_ppm_saved.c_str());
    unlink(yeet_ppm.c_str());
    rmdir(temp_dir);
}

TEST(RealworldCodebaseTest, Sha256BenchmarkRunsIdentically) {
    std::string sha_source = ReadFileToString("tests/testdata/sha256_benchmark.cpp");
    ASSERT_FALSE(sha_source.empty());

    Transformer transformer(999);
    TransformResult result = transformer.Transform(sha_source, "sha256_benchmark.cpp");
    ASSERT_FALSE(result.yeetified_code.empty());

    char temp_dir[] = "/tmp/yeet_sha_XXXXXX";
    ASSERT_NE(mkdtemp(temp_dir), nullptr);

    std::string orig_cpp = std::string(temp_dir) + "/orig.cpp";
    std::string yeet_cpp = std::string(temp_dir) + "/yeet.cpp";
    std::string orig_bin = std::string(temp_dir) + "/orig_bin";
    std::string yeet_bin = std::string(temp_dir) + "/yeet_bin";

    {
        std::ofstream f(orig_cpp);
        f << sha_source;
    }
    {
        std::ofstream f(yeet_cpp);
        f << result.yeetified_code;
    }

    int orig_compile_rc = 0;
    std::string orig_compile_out = RunCommand("clang++ -std=c++17 -O3 " + orig_cpp + " -o " + orig_bin, orig_compile_rc);
    ASSERT_EQ(orig_compile_rc, 0) << orig_compile_out;

    int yeet_compile_rc = 0;
    std::string yeet_compile_out = RunCommand("clang++ -std=c++17 -O3 " + yeet_cpp + " -o " + yeet_bin, yeet_compile_rc);
    ASSERT_EQ(yeet_compile_rc, 0) << yeet_compile_out;

    int orig_run_rc = 0;
    std::string orig_stdout = RunCommand(orig_bin, orig_run_rc);
    EXPECT_EQ(orig_run_rc, 0);

    int yeet_run_rc = 0;
    std::string yeet_stdout = RunCommand(yeet_bin, yeet_run_rc);
    EXPECT_EQ(yeet_run_rc, 0);

    EXPECT_FALSE(orig_stdout.empty());
    EXPECT_EQ(orig_stdout, yeet_stdout)
        << "Cryptographic SHA256 hashes produced by yeetified binary do not match original!";

    unlink(orig_cpp.c_str());
    unlink(yeet_cpp.c_str());
    unlink(orig_bin.c_str());
    unlink(yeet_bin.c_str());
    rmdir(temp_dir);
}

}  // namespace
}  // namespace yeetify
