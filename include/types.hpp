#pragma once

#include <cstdint>
#include <string>
#include <filesystem>

struct DirEntry {
    std::string relativePath;
    unsigned int permissions = 0;
};

struct FileEntry {
    std::filesystem::path relativePath;
    std::string fullPath;
    uint64_t size = 0;
    uint32_t permissions = 0;
    uint64_t hash = 0;
    std::filesystem::path source_example;
};
