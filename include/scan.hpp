#pragma once

#include "types.hpp"

#include <cstdint>
#include <vector>
#include <filesystem>

struct ScannedFile {
    std::string relativePath;
    uint64_t size = 0;
    u_int32_t permissions = 0;
};


class IScanner {
public:
    virtual ~IScanner() = default;
    virtual void scan(const std::filesystem::path& root,
              std::vector<DirEntry>& out_dirs,
              std::vector<ScannedFile>& out_files) = 0;
};

class FsScanner : public IScanner {
public:
    void scan(const std::filesystem::__cxx11::path &root, std::vector<DirEntry> &outputDirs, std::vector<ScannedFile> &outputFiles);
};
