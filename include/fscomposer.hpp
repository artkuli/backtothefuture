#pragma once

#include "archivereader.hpp"

#include <filesystem>
#include <vector>
#include <cstdint>
#include <string>


class FSComposer {
public:
    explicit FSComposer(const std::size_t chunkSize = 1u << 20);

    void composeDirectories(const std::vector<DirInfo>& dirs,
                           const std::filesystem::path& root) const;

    void composeFiles(IArchiveReader& archive,
                    const std::vector<FileInfo>& files,
                    const std::vector<BlobRecord>& blobs,
                    const std::filesystem::path& root) const;
private:
    size_t m_chunkSize;
    static std::filesystem::path resolvePath(const std::filesystem::path& root,
                                                   const std::string& relative);
    void applyPermissions(const std::filesystem::path& p, uint32_t mode) const;
    void applyMtime(const std::filesystem::path& p, uint64_t mtime) const;
};
