#include "pack.hpp"

#include "scan.hpp"
#include "hasher.hpp"
#include "filescomparer.h"
#include "duplicateresolver.h"

#include <iostream>

#include <vector>
#include <future>
#include <cstring>

namespace packager
{

int pack(const std::filesystem::path &input_dir, const std::filesystem::path &archive_path)
{
    std::vector<FileEntry> files;
    std::vector<DirEntry> dirs;
    std::unordered_map<uint64_t, std::vector<size_t>> filesBySize;

    FsScanner scanner;
    scanner.scan(input_dir, dirs, files, filesBySize);

    DefaultFileComparer filesComparer;
    FNV1aHasher hasher;

    DuplicateResolver duplicateResolver(hasher, filesComparer);
    duplicateResolver.resolve(input_dir, files, filesBySize);

    for (const auto &file : files){
        std::cout << "Path: " << file.relativePath
                  << " full=" << file.fullPath
                  << " hash=" << file.hash
                  << " size=" << file.size
                  << " perms=" << file.permissions
                  << " source=" << (file.source_example.empty() ? "SOURCE" : file.source_example.string())
                  << std::endl;
    }
    return 0;
}

}
