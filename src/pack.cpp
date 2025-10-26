#include "pack.hpp"

#include "archiver.hpp"
#include "scan.hpp"
#include "hasher.hpp"
#include "filescomparer.hpp"
#include "duplicateresolver.hpp"

#include <iostream>

#include <vector>
#include <cstring>

namespace packager
{

int pack(const std::filesystem::path &input_dir, const std::filesystem::path &archive_path)
{
    std::vector<FileEntry> files;
    std::vector<DirEntry> dirs;
    std::vector<BlobInfo> blobs;
    std::unordered_map<uint64_t, std::vector<size_t>> filesBySize;

    FsScanner scanner;
    scanner.scan(input_dir, dirs, files, filesBySize);

    std::cout << "Scanned " << dirs.size() << " dirs and " << files.size() << " files." << std::endl;

    DefaultFileComparer filesComparer;
    FNV1aHasher hasher;

    DuplicateResolver duplicateResolver(hasher, filesComparer);
    duplicateResolver.resolve(input_dir, files, filesBySize, blobs);

    int idx = 0;
    for (const auto &file : files){
        std::cout <<"[" << idx << "]= "<< "Path: " << file.relativePath
                  << " full=" << file.fullPath
                  << " hash=" << blobs[file.blobId].hash64
                  << " size=" << blobs[file.blobId].size
                  << " perms=" << file.permissions
                  << " source=" << blobs[file.blobId].source.string()
                  << std::endl;
        ++idx;
    }
    idx = 0;
    for (const auto &blob : blobs){
        std::cout <<"[" << idx << "]= "<< "Blob: size=" << blob.size
                  << " hash=" << blob.hash64
                  << " source=" << blob.source.string()
                  << std::endl;
        ++idx;
    }

    Archiver archiver;
    archiver.archive(dirs, files, blobs, input_dir, archive_path);

    return 0;
}

}
