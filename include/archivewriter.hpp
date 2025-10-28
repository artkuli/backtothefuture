#pragma once

#include "writer.hpp"

#include <vector>
#include <filesystem>

struct DirInfo;
struct Header;
struct FileInfo;
struct BlobInfo;

class IArchiveWriter {
public:
    virtual ~IArchiveWriter() = default;
    virtual bool isOpen() const = 0;
    virtual std::streampos currentPos()= 0;

    virtual void writeHeader(const Header& header, std::streampos& position) = 0;
    virtual void writeDirs(const std::vector<DirInfo>& dirs) = 0;
    virtual void writeFiles(const std::vector<FileInfo> &files) = 0;
    virtual void writeBlobs(const std::filesystem::path &inputDir,
                                const std::vector<BlobInfo>& blobs,
                                std::vector<std::streampos> &blobOffsetPlaceHolders) = 0;
    virtual std::vector<std::streampos> writeBlobsTable(const std::vector<BlobInfo>& blobs) = 0;
};

class BinaryArchiveWriter : public IArchiveWriter {
public:
    explicit BinaryArchiveWriter(const std::filesystem::path& outPath, size_t chunkSize);

    bool isOpen() const override;
    std::streampos currentPos() override;
    void writeHeader(const Header& header, std::streampos& position) override;
    void writeDirs(const std::vector<DirInfo>& dirs) override;
    void writeFiles(const std::vector<FileInfo> &files) override;
    void writeBlobs(const std::filesystem::path &inputDir,
                    const std::vector<BlobInfo>& blobs,
                    std::vector<std::streampos> &blobOffsetPlaceHolders) override;
    std::vector<std::streampos> writeBlobsTable(const std::vector<BlobInfo>& blobs) override;
private:
    Writer m_writer;
    std::ofstream m_stream;
    size_t m_chunkSize = 1u << 20;
};
