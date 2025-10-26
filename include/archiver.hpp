#pragma once

#include <vector>
#include <filesystem>

struct DirEntry;
struct FileEntry;
struct BlobInfo;

class Archiver {
public:
    Archiver() = default;

    bool archive(const std::vector<DirEntry> &dirs,
                 const std::vector<FileEntry> &files,
                 const std::vector<BlobInfo> &blobs,
                 const std::filesystem::path &inputDir,
                 const std::filesystem::path &archivePath);
};
