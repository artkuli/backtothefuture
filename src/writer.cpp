#include "writer.hpp"
#include "types.hpp"

const size_t CHUNK = 1<<20;


Writer::Writer(const std::filesystem::path &outPath)
    : m_out(outPath, std::ios::binary)
{

}

std::streampos Writer::currentPos()
{
    return m_out.tellp();
}

bool Writer::isOpen() const
{
    return m_out.is_open();
}

void Writer::writeHeader(const uint64_t dirCount, const uint64_t fileCount, const uint64_t blobCount)
{
    m_header.fileCount = fileCount;
    m_header.dirCount = dirCount;
    m_header.blobCount = blobCount;
    write_u32(m_header.version);
    write_u64(m_header.dirCount);
    write_u64(m_header.fileCount);
    write_u64(m_header.blobCount);
    write_u64(m_header.dirTableOff);
    write_u64(m_header.fileTableOff);
    write_u64(m_header.blobTableOff);
    write_u64(m_header.blobDataOff);
}

void Writer::writeDirs(const std::vector<DirEntry> &dirs)
{
    for (const auto& d : dirs)
    {
        write_string(d.relativePath);
        write_u32(static_cast<uint32_t>(d.permissions));
        write_u64(d.mtime);
    }
}

void Writer::writeFiles(const std::vector<FileEntry> &files)
{
    for (const auto& f : files)
    {
        write_string(f.relativePath.generic_u8string());
        write_u32(f.permissions);
        write_u64(f.size);
        write_u64(f.mtime);
        write_u64(f.blobId);
    }
}

void Writer::writeBlobs(const std::filesystem::path &inputDir, const std::vector<BlobInfo> &blobs)
{
    auto blobTableOffset = uint64_t(currentPos());
    const auto blobCount = blobs.size();
    std::vector<std::streampos> blobOffsetPlaceholders;
    blobOffsetPlaceholders.reserve(blobCount);

    for(const auto& b: blobs){
        write_u64(b.size);
        write_u64(b.hash64);
        blobOffsetPlaceholders.push_back(currentPos());
        write_u64(0ULL); // placeholder dataOff
    }

    auto blobDataOffset = currentPos();
    std::vector<uint64_t> dataOffsets(blobCount, 0);

    for(size_t i=0;i<blobCount;++i) {
        dataOffsets[i] = uint64_t(currentPos());
        std::ifstream in(inputDir / blobs[i].source, std::ios::binary);
        if(!in) {
            throw std::runtime_error("An error occured while tried to open the file: " + blobs[i].source.string());
        }
        copy_bytes(in, blobs[i].size);
    }

    for(size_t i=0;i<blobCount;++i) {
        m_out.seekp(blobOffsetPlaceholders[i]);
        write_u64(dataOffsets[i]);
    }
}

void Writer::updateOffsets(const std::streampos headerPos, const std::streampos dirTableOffset, const std::streampos fileTableOffset, const std::streampos blobTableOffset)
{
    m_out.seekp(headerPos);
    m_header.dirTableOff = dirTableOffset;
    m_header.fileTableOff = fileTableOffset;
    m_header.blobTableOff = blobTableOffset;
    writeHeader(m_header.dirCount, m_header.fileCount, m_header.blobCount);
}

void Writer::write_u64(uint64_t v)
{
    unsigned char b[8];
    for (int i=0;i<8;i++)
    {
        b[i]=(unsigned char)(v>>(8*i));
    }
    m_out.write((char*)b,8);
}
void Writer::write_u32(uint32_t v)
{
    unsigned char b[4]={
        (unsigned char)(v),
        (unsigned char)(v>>8),
        (unsigned char)(v>>16),
        (unsigned char)(v>>24)};
    m_out.write((char*)b,4);
}
void Writer::write_string(const std::string &s)
{
    write_u32(static_cast<uint32_t>(s.size()));
    m_out.write((const char *)s.data(), std::streamsize(s.size()));
}

void Writer::copy_bytes(std::ifstream &in, uint64_t n) {

    std::vector<char> buf(CHUNK);
    uint64_t left=n;
    while(left){
        size_t take = size_t(std::min<uint64_t>(left, CHUNK));
        in.read(buf.data(), std::streamsize(take));
        if(!in)
            throw std::runtime_error("Error of reading input");
        m_out.write(buf.data(), std::streamsize(take));
        left -= take;
    }
}
