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
              std::vector<DirInfo>& out_dirs,
              std::vector<FileInfo>& out_files,
              std::unordered_map<uint64_t, std::vector<size_t>>& filesBySize) = 0;
};

class FsScanner : public IScanner {
public:
    explicit FsScanner(uint64_t maxFileSize);
    void scan(const std::filesystem::path &root, std::vector<DirInfo> &outputDirs, std::vector<FileInfo> &outputFiles, std::unordered_map<uint64_t, std::vector<size_t>>& filesBySize) override;
private:
    uint64_t m_maxFileSize = 0;
};
