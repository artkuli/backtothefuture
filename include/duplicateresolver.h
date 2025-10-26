#pragma once

#include "hasher.hpp"
#include "filescomparer.h"
#include "types.hpp"

#include <vector>
#include <unordered_map>

class DuplicateResolver
{
public:
    DuplicateResolver(IHasher& hasher, IFilesComparer &cfilesComparer);

    void resolve(const std::filesystem::path& inputDir, std::vector<FileEntry> &files, const std::unordered_map<uint64_t, std::vector<size_t>> &filesBySize);
private:
    IHasher& m_hasher;
    IFilesComparer& m_filesComparer;
};
