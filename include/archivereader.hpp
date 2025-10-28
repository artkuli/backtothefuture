#pragma once

#include "reader.hpp"
#include <vector>

struct Header;
struct DirInfo;
struct FileInfo;

struct BlobRecord {
    uint64_t size = 0;
    uint64_t hash64 = 0;
    uint64_t dataOffset = 0;
};

class IArchiveReader {
public:
    virtual ~IArchiveReader() = default;
    virtual bool isOpen() const = 0;
    virtual Header readHeader() = 0;
    virtual std::vector<DirInfo> readDirs(const Header& header) = 0;
    virtual std::vector<FileInfo> readFiles(const Header& header) = 0;
    virtual std::vector<BlobRecord> readBlobs(const Header& header) = 0;
    virtual std::unique_ptr<std::istream> openBlobStream(uint64_t dataOffset) = 0;
};

class BinaryArchiveReader final : public IArchiveReader {
public:
    explicit BinaryArchiveReader(const std::filesystem::path& archivePath);

    bool isOpen() const override;
    Header readHeader() override;
    std::vector<DirInfo> readDirs(const Header& header) override;
    std::vector<FileInfo> readFiles(const Header& header) override;
    std::vector<BlobRecord> readBlobs(const Header& header) override;
    std::unique_ptr<std::istream> openBlobStream(uint64_t dataOffset) override;

private:
    Reader m_reader;
    std::ifstream m_stream;
    std::filesystem::path m_path;
};
