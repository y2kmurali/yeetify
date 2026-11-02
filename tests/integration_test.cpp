#include "src/transformer.h"

#include <gtest/gtest.h>
#include <array>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <string>
#include <unistd.h>

namespace yeetify {
namespace {

// Helper to execute a command and capture stdout
std::string ExecuteCommand(const std::string& command, int& exit_code) {
    std::array<char, 128> buffer;
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

TEST(IntegrationTest, CompilesAndExecutesYeetifiedHelloWorldWithSpaces) {
    Transformer transformer(1337);

    std::string original_code =
        "#include <iostream>\n"
        "int main() {\n"
        "    std::cout << \"Hello from yeetified C++! 1 2 3\" << std::endl;\n"
        "    return 0;\n"
        "}\n";

    auto result = transformer.Transform(original_code, "test_hello.cpp");
    ASSERT_FALSE(result.yeetified_code.empty());

    // Write to a temporary file
    char temp_cpp[] = "/tmp/yeet_test_XXXXXX.cpp";
    int fd = mkstemps(temp_cpp, 4);
    ASSERT_GE(fd, 0);
    close(fd);

    std::ofstream out(temp_cpp);
    out << result.yeetified_code;
    out.close();

    std::string temp_bin = std::string(temp_cpp) + ".out";

    // Compile the yeetified file
    int compile_exit = 0;
    std::string compile_cmd = "clang++ -std=c++17 " + std::string(temp_cpp) + " -o " + temp_bin;
    std::string compile_output = ExecuteCommand(compile_cmd, compile_exit);
    EXPECT_EQ(compile_exit, 0) << "Compilation failed:\n" << compile_output
                              << "\n\nYeetified Code:\n" << result.yeetified_code;

    // Run the compiled binary
    if (compile_exit == 0) {
        int run_exit = 0;
        std::string run_output = ExecuteCommand(temp_bin, run_exit);
        EXPECT_EQ(run_exit, 0);
        EXPECT_EQ(run_output, "Hello from yeetified C++! 1 2 3\n");
    }

    // Clean up
    unlink(temp_cpp);
    unlink(temp_bin.c_str());
}

TEST(IntegrationTest, CompilesAndExecutesArithmeticWithoutSpaces) {
    Transformer transformer(2024);

    std::string original_code =
        "#include <iostream>\n"
        "int add(int a,int b){return a+b;}\n"
        "int main(){\n"
        "    int x=10;\n"
        "    int y=20;\n"
        "    std::cout<<(add(x,y)*2)<<std::endl;\n"
        "    return 0;\n"
        "}\n";

    auto result = transformer.Transform(original_code, "test_arith.cpp");
    ASSERT_FALSE(result.yeetified_code.empty());

    char temp_cpp[] = "/tmp/yeet_arith_XXXXXX.cpp";
    int fd = mkstemps(temp_cpp, 4);
    ASSERT_GE(fd, 0);
    close(fd);

    std::ofstream out(temp_cpp);
    out << result.yeetified_code;
    out.close();

    std::string temp_bin = std::string(temp_cpp) + ".out";

    int compile_exit = 0;
    std::string compile_cmd = "clang++ -std=c++17 " + std::string(temp_cpp) + " -o " + temp_bin;
    std::string compile_output = ExecuteCommand(compile_cmd, compile_exit);
    EXPECT_EQ(compile_exit, 0) << "Compilation failed:\n" << compile_output
                              << "\n\nYeetified Code:\n" << result.yeetified_code;

    if (compile_exit == 0) {
        int run_exit = 0;
        std::string run_output = ExecuteCommand(temp_bin, run_exit);
        EXPECT_EQ(run_exit, 0);
        EXPECT_EQ(run_output, "60\n");
    }

    unlink(temp_cpp);
    unlink(temp_bin.c_str());
}

}  // namespace
}  // namespace yeetify
