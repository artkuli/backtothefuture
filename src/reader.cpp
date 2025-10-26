#include "reader.hpp"

#include "types.hpp"


Reader::Reader(const std::filesystem::__cxx11::path &inputPath) : m_in(inputPath)
{

}

bool Reader::isOpen() const
{
    return m_in.is_open();
}

std::streampos Reader::currentPos()
{
    return m_in.tellg();
}

Header Reader::readHeader()
{
    Header header;
    header.version = read_u32(m_in);
    if(header.version != 1) {
        throw std::runtime_error("Nieobsługiwana wersja");
    }
    header.dirCount = read_u64(m_in);
    header.fileCount = read_u64(m_in);
    header.blobCount = read_u64(m_in);
    header.dirTableOff  = read_u64(m_in);
    header.fileTableOff = read_u64(m_in);
    header.blobTableOff = read_u64(m_in);
    header.blobDataOff  = read_u64(m_in);
    return header;
}

std::ifstream& Reader::stream()
{
    return m_in;
}

uint32_t Reader::read_u32(std::ifstream &stream)
{
    unsigned char b[4];
    stream.read((char*)b,4);
    if(!stream) {
        throw std::runtime_error("Corrupted Archive");
    }
    return (uint32_t)b[0]
           | ((uint32_t)b[1]<<8)
           | ((uint32_t)b[2]<<16)
           | ((uint32_t)b[3]<<24);
}

uint64_t Reader::read_u64(std::ifstream &stream)
{
    unsigned char b[8];
    stream.read((char*)b,8);
    if(!stream)
    {
        throw std::runtime_error("Corrupted Archive");
    }
    uint64_t v=0;
    for(int i=0;i<8;i++) {
        v |= ((uint64_t)b[i]) << (8 * i);
    }
    return v;
}

std::string Reader::read_string(std::ifstream &stream)
{
    uint32_t n = read_u32(stream);
    std::string s(n, '\0');
    m_in.read(&s[0], std::streamsize(n));
    return s;
}
