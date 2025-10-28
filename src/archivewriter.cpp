#include "archivewriter.hpp"

#include "types.hpp"
#include <ostream>

BinaryArchiveWriter::BinaryArchiveWriter(const std::filesystem::path &outPath, size_t chunkSize)
    : IArchiveWriter()
    , m_stream(outPath, std::ios::binary | std::ios::trunc)
    , m_chunkSize(chunkSize)
{
}

bool BinaryArchiveWriter::isOpen() const
{
    return m_stream.is_open();
}

std::streampos BinaryArchiveWriter::currentPos()
{
    return m_stream.tellp();
}

void BinaryArchiveWriter::writeHeader(const Header &header, std::streampos &position)
{
    m_stream.seekp(position);
    m_writer.writeU32(header.version, m_stream);
    m_writer.writeU64(header.dirCount, m_stream);
    m_writer.writeU64(header.fileCount, m_stream);
    m_writer.writeU64(header.blobCount, m_stream);  
    m_writer.writeU64(header.dirTableOffset, m_stream);
    m_writer.writeU64(header.fileTableOffset, m_stream);
    m_writer.writeU64(header.blobTableOffset, m_stream);
    m_writer.writeU64(header.blobDataOffset, m_stream);
}

void BinaryArchiveWriter::writeDirs(const std::vector<DirInfo> &dirs)
{
    for (const auto& d : dirs)
    {
        m_writer.writeString(d.relativePath, m_stream);
        m_writer.writeU32(static_cast<uint32_t>(d.permissions), m_stream);
        m_writer.writeU64(d.mtime, m_stream);
    }
}

void BinaryArchiveWriter::writeFiles(const std::vector<FileInfo> &files)
{
    for (const auto& f : files)
    {
        m_writer.writeString(f.relativePath.generic_u8string(), m_stream);
        m_writer.writeU32(f.permissions, m_stream);
        m_writer.writeU64(f.size, m_stream);
        m_writer.writeU64(f.mtime, m_stream);
        m_writer.writeU64(f.blobId, m_stream);
    }
}

void BinaryArchiveWriter::writeBlobs(const std::filesystem::path &inputDir, const std::vector<BlobInfo> &blobs, std::vector<std::streampos> &blobOffsetPlaceHolders)
{
    const auto blobCount = blobs.size();
    std::vector<uint64_t> dataOffsets(blobCount, 0);

    for(size_t i=0;i<blobCount;++i) {
        dataOffsets[i] = uint64_t(currentPos());
        std::ifstream in(inputDir / blobs[i].source, std::ios::binary);
        if(!in) {
            throw std::runtime_error("An error occured while tried to open the file: " + blobs[i].source.string());
        }
        m_writer.copyBytes(in, blobs[i].size, m_stream, m_chunkSize);
    }

    for(size_t i=0;i<blobCount;++i) {
        m_stream.seekp(blobOffsetPlaceHolders[i]);
        m_writer.writeU64(dataOffsets[i], m_stream);
    }
}

std::vector<std::streampos> BinaryArchiveWriter::writeBlobsTable(const std::vector<BlobInfo> &blobs)
{
    const auto blobCount = blobs.size();
    std::vector<std::streampos> blobOffsetPlaceholders;
    blobOffsetPlaceholders.reserve(blobCount);

    for(const auto& b: blobs){
        m_writer.writeU64(b.size, m_stream);
        m_writer.writeU64(b.hash64, m_stream);
        blobOffsetPlaceholders.push_back(currentPos());
        m_writer.writeU64(0ULL, m_stream); // placeholder dataOffset
    }
    return blobOffsetPlaceholders;

}


