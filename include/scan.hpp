#pragma once

#include "types.hpp"

#include <cstdint>
#include <vector>
#include <filesystem>
#include <unordered_map>

class IScanner {
public:
    virtual ~IScanner() = default;
    virtual void scan(const std::filesystem::path& root,
              std::vector<DirEntry>& out_dirs,
              std::vector<FileEntry>& out_files,
              std::unordered_map<uint64_t, std::vector<size_t>>& filesBySize) = 0;
};

class FsScanner : public IScanner {
public:
    explicit FsScanner(uint64_t maxFileSize = 2ull * 1024 * 1024 * 1024);
    void scan(const std::filesystem::path &root, std::vector<DirEntry> &outputDirs, std::vector<FileEntry> &outputFiles, std::unordered_map<uint64_t, std::vector<size_t>>& filesBySize) override;
private:
    uint64_t m_maxFileSize = 0;
};
