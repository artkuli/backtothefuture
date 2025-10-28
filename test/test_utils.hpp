#pragma once

#include <filesystem>
#include <vector>

std::string bytes(std::initializer_list<uint8_t> b);

std::filesystem::path makeTempDir(const std::string& stem);

struct TempFile {
    std::filesystem::path path;
    TempFile(const std::string& stem, const std::vector<uint8_t>& data);
    ~TempFile();
};
