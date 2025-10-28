#pragma once

#include "config.hpp"
#include "hasher.hpp"
#include "filescomparer.hpp"
#include "types.hpp"

#include <vector>
#include <unordered_map>

class DuplicateResolver
{
public:
    DuplicateResolver(const Config& config, IFilesComparer &cfilesComparer);
    
    void resolve(const std::filesystem::path& inputDir, std::vector<FileInfo> &files, const std::unordered_map<uint64_t, std::vector<size_t>> &filesBySize, std::vector<BlobInfo> &blobs);
private:
    const Config& m_config;
    IFilesComparer& m_filesComparer;
};
