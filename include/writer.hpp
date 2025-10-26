#pragma once

#include "types.hpp"

#include <filesystem>
#include <fstream>
#include <vector>

class Writer
{
public:
    Writer(const std::filesystem::path &outPath);
    std::streampos currentPos();

    bool isOpen() const;
    void writeHeader(const uint64_t dirCount, const uint64_t fileCount, const uint64_t blobCount);
    void writeDirs(const std::vector<DirEntry> &dirs);
    void writeFiles(const std::vector<FileEntry> &files);
    void writeBlobs(const std::filesystem::path &inputDir, const std::vector<BlobInfo>& blobs);
    void updateOffsets(const std::streampos headerPos,
                       const std::streampos dirTableOffset,
                       const std::streampos fileTableOffset,
                       const std::streampos blobTableOffset);
private:
    Header m_header;
    std::ofstream m_out;

    void write_u32(uint32_t v);
    void write_u64(uint64_t v);
    void write_string(const std::string &s);
    void copy_bytes(std::ifstream& in, uint64_t n);
};
