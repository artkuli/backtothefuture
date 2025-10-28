#include "reader.hpp"
#include <limits>

namespace {

template <typename T>
void readExact(std::istream& is, T* ptr, std::size_t bytes) {
    is.read(reinterpret_cast<char*>(ptr), static_cast<std::streamsize>(bytes));
    if (!is)
    {
        throw std::runtime_error("Corrupted Archive");
    }
}

inline uint32_t le32(const unsigned char b[4]) {
    return (uint32_t)b[0]
           | ((uint32_t)b[1] << 8)
           | ((uint32_t)b[2] << 16)
           | ((uint32_t)b[3] << 24);
}

inline uint64_t le64(const unsigned char b[8]) {
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i) {
        v |= (uint64_t)b[i] << (8 * i);
    }
    return v;
}

} // anonymous namespace


uint32_t Reader::readU32(std::istream& stream)
{
    unsigned char b[4];
    readExact(stream, b, 4);
    return le32(b);
}

uint64_t Reader::readU64(std::istream& stream) {
    unsigned char b[8];
    readExact(stream, b, 8);
    return le64(b);
}

std::string Reader::readString(std::istream& stream) {
    const uint32_t n = readU32(stream);
    std::string s;
    s.resize(n);
    if (n) {
        readExact(stream, &s[0], n);
    }
    return s;
}
