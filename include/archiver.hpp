#pragma once

#include <vector>
#include <filesystem>
#include "archivewriter.hpp"

struct DirInfo;
struct FileInfo;
struct BlobInfo;

class Archiver {
public:
    Archiver(IArchiveWriter& writer);

    bool archive(const std::vector<DirInfo> &dirs,
                 const std::vector<FileInfo> &files,
                 const std::vector<BlobInfo> &blobs,
                 const std::filesystem::path &inputDir);
private:
    IArchiveWriter& m_archiveWriter;
};
