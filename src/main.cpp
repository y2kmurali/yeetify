#include "src/transformer.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

namespace {

void PrintUsage(const char* prog_name) {
    std::cout << "Usage: " << prog_name << " <source_file> [-o <output_file>] [--seed <number>]\n\n"
              << "Options:\n"
              << "  -o <path>       Specify custom output path (default: <basename>_yeetified<ext>)\n"
              << "  --seed <number> Specify random seed for reproducible yeet generation\n"
              << "  -h, --help      Display this help message\n";
}

std::string GetExtension(const std::string& path) {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos == std::string::npos) {
        return "";
    }
    return path.substr(dot_pos);
}

std::string GetBasename(const std::string& path) {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos == std::string::npos) {
        return path;
    }
    return path.substr(0, dot_pos);
}

bool IsValidExtension(const std::string& ext) {
    return ext == ".cpp" || ext == ".cc" || ext == ".c" ||
           ext == ".cxx" || ext == ".hpp" || ext == ".h";
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc < 2) {
        PrintUsage(argv[0]);
        return 1;
    }

    std::string input_path;
    std::string output_path;
    unsigned int seed = static_cast<unsigned int>(
        std::chrono::system_clock::now().time_since_epoch().count());

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            PrintUsage(argv[0]);
            return 0;
        } else if (arg == "-o" && i + 1 < argc) {
            output_path = argv[++i];
        } else if (arg == "--seed" && i + 1 < argc) {
            seed = static_cast<unsigned int>(std::stoul(argv[++i]));
        } else if (arg[0] == '-') {
            std::cerr << "Unknown option: " << arg << "\n";
            PrintUsage(argv[0]);
            return 1;
        } else if (input_path.empty()) {
            input_path = arg;
        } else {
            std::cerr << "Unexpected extra argument: " << arg << "\n";
            PrintUsage(argv[0]);
            return 1;
        }
    }

    if (input_path.empty()) {
        std::cerr << "Error: No input file specified.\n";
        PrintUsage(argv[0]);
        return 1;
    }

    std::string ext = GetExtension(input_path);
    if (!IsValidExtension(ext)) {
        std::cerr << "Error: Invalid file extension '" << ext
                  << "'. Expected .c, .cc, .cpp, .cxx, .h, or .hpp.\n";
        return 1;
    }

    std::ifstream in(input_path);
    if (!in.is_open()) {
        std::cerr << "Error: Could not open input file '" << input_path << "'\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << in.rdbuf();
    std::string source_code = buffer.str();
    in.close();

    if (output_path.empty()) {
        output_path = GetBasename(input_path) + "_yeetified" + ext;
    }

    yeetify::Transformer transformer(seed);
    yeetify::TransformResult result = transformer.Transform(source_code, input_path);

    std::ofstream out(output_path);
    if (!out.is_open()) {
        std::cerr << "Error: Could not open output file '" << output_path << "'\n";
        return 1;
    }

    out << result.yeetified_code;
    out.close();

    std::cout << "Successfully yeetified '" << input_path << "' -> '"
              << output_path << "' (" << result.token_to_yeet.size()
              << " unique tokens replaced).\n";

    return 0;
}
