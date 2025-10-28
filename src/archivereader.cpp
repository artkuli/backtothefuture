#include "archivereader.hpp"

#include "types.hpp"


BinaryArchiveReader::BinaryArchiveReader(const std::filesystem::path& archivePath)
    : m_stream(archivePath, std::ios::binary)
    , m_path(archivePath)
{
}

bool BinaryArchiveReader::isOpen() const
{
    return m_stream.is_open();
}

Header BinaryArchiveReader::readHeader()
{
    m_stream.seekg(0, std::ios::beg);

    Header h{};
    h.version = m_reader.readU32(m_stream);
    if (h.version != 1) {
        throw std::runtime_error("Unsupported version of file");
    }

    h.dirCount = m_reader.readU64(m_stream);
    h.fileCount = m_reader.readU64(m_stream);
    h.blobCount = m_reader.readU64(m_stream);
    h.dirTableOffset = m_reader.readU64(m_stream);
    h.fileTableOffset = m_reader.readU64(m_stream);
    h.blobTableOffset = m_reader.readU64(m_stream);
    h.blobDataOffset = m_reader.readU64(m_stream);
    return h;
}

std::vector<DirInfo> BinaryArchiveReader::readDirs(const Header& header)
{
    m_stream.seekg(static_cast<std::streamoff>(header.dirTableOffset), std::ios::beg);
    std::vector<DirInfo> dirs;
    dirs.reserve(static_cast<size_t>(header.dirCount));
    for (uint64_t i = 0; i < header.dirCount; ++i) {
        DirInfo d;
        d.relativePath = m_reader.readString(m_stream);
        d.permissions = m_reader.readU32(m_stream);
        d.mtime = m_reader.readU64(m_stream);
        dirs.emplace_back(std::move(d));
    }
    return dirs;
}

std::vector<FileInfo> BinaryArchiveReader::readFiles(const Header& header)
{
    m_stream.seekg(static_cast<std::streamoff>(header.fileTableOffset), std::ios::beg);
    std::vector<FileInfo> files;
    files.reserve(static_cast<size_t>(header.fileCount));
    for (uint64_t i = 0; i < header.fileCount; ++i) {
        FileInfo f;
        f.relativePath = m_reader.readString(m_stream);
        f.permissions = m_reader.readU32(m_stream);
        f.size = m_reader.readU64(m_stream);
        f.mtime = m_reader.readU64(m_stream);
        f.blobId = m_reader.readU64(m_stream);
        if (f.blobId >= header.blobCount) {
            throw std::runtime_error("Corrupted Archive (invalid blobId)");
        }
        files.emplace_back(std::move(f));
    }
    return files;
}

std::vector<BlobRecord> BinaryArchiveReader::readBlobs(const Header& header)
{
    m_stream.seekg(static_cast<std::streamoff>(header.blobTableOffset), std::ios::beg);
    std::vector<BlobRecord> blobs;
    blobs.reserve(static_cast<size_t>(header.blobCount));
    for (uint64_t i = 0; i < header.blobCount; ++i) {
        BlobRecord br;
        br.size = m_reader.readU64(m_stream);
        br.hash64 = m_reader.readU64(m_stream);
        br.dataOffset = m_reader.readU64(m_stream);
        if (br.dataOffset == 0 && br.size > 0) {
            throw std::runtime_error("Empty blob dataOffset for non-empty blob");
        }
        blobs.emplace_back(std::move(br));
    }
    return blobs;
}

std::unique_ptr<std::istream> BinaryArchiveReader::openBlobStream(uint64_t dataOffset)
{
    auto is = std::make_unique<std::ifstream>(m_path, std::ios::binary);
    if (!*is) {
        throw std::runtime_error("Cannot open archive for blob reading");
    }
    is->seekg(static_cast<std::streamoff>(dataOffset), std::ios::beg);

    if (!*is) {
        throw std::runtime_error("Not able to seek to blob data offset");
    }
    return is;
}
