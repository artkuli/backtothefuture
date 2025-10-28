#include "writer.hpp"

#include <vector>

void Writer::writeU64(uint64_t v, std::ostream& stream)
{
    unsigned char b[8];
    for (int i=0;i<8;i++)
    {
        b[i]=(unsigned char)(v>>(8*i));
    }
    stream.write((char*)b,8);
}

void Writer::writeU32(uint32_t v, std::ostream& stream)
{
    unsigned char b[4]={
        (unsigned char)(v),
        (unsigned char)(v>>8),
        (unsigned char)(v>>16),
        (unsigned char)(v>>24)};
    stream.write((char*)b,4);
}

void Writer::writeString(const std::string &s, std::ostream& stream)
{
    writeU32(static_cast<uint32_t>(s.size()), stream);
    stream.write((const char *)s.data(), std::streamsize(s.size()));
}

void Writer::copyBytes(std::ifstream &in, uint64_t n, std::ostream& out, size_t chunkSize) {

    std::vector<char> buf(chunkSize);
    uint64_t left=n;
    while(left){
        size_t take = size_t(std::min<uint64_t>(left, chunkSize));
        in.read(buf.data(), std::streamsize(take));
        if(!in)
            throw std::runtime_error("Error of reading input");
        out.write(buf.data(), std::streamsize(take));
        left -= take;
    }
}
