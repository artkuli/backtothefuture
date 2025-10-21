#pragma once

#include "pack.hpp"
#include "unpack.hpp"

#include <iostream>
#include <filesystem>
#include <string_view>

const std::string_view kPackCmd = "pack";
const std::string_view kUnpackCmd = "unpack";
const int kInvalidUsage = 2;

void printHelp(const std::string& appName)
{
    std::cerr << "Usage:\n"
              << "  " << appName << " pack <input_dir> <archive.bttf>\n"
              << "  " << appName << " unpack <archive.bttf> <output_dir>\n";
}

int main(int argc, char** argv) {
    try {
        if (argc < 2){
            printHelp(argv[0]);
            return kInvalidUsage;
        }
        std::string cmd = argv[1];
        if (cmd == kPackCmd){
            if (argc != 4) {
                std::cerr << "pack needs <input_dir> <archive.bttf>\n";
                return kInvalidUsage;
            }
            std::filesystem::path in = std::filesystem::path(argv[2]);
            std::filesystem::path out= std::filesystem::path(argv[3]);

            return Packager::pack(in, out);
        } else if (cmd == kUnpackCmd){
            if (argc != 4) {
                std::cerr << "unpack needs <archive.bttf> <output_dir>\n";
                return kInvalidUsage;;
            }
            std::filesystem::path in = std::filesystem::path(argv[2]);
            std::filesystem::path out= std::filesystem::path(argv[3]);
            return Unpackager::unpack(in, out);
        } else {
            std::cerr << "Unknown command: " << cmd << "\n";
            printHelp(argv[0]);
            return kInvalidUsage;
        }
    } catch (const std::exception& e){
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
}
