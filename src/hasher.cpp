#include "hasher.hpp"

#include <fstream>
#include <vector>

constexpr uint64_t FNV1aOffset = 0xcbf29ce484222325ull;
constexpr uint64_t FNV1aPrime  = 0x00000100000001b3ull;

FNV1aHasher::FNV1aHasher(size_t chunkSize)
    : m_chunkSize(chunkSize)
{
}

uint64_t FNV1aHasher::hash_file(const std::filesystem::path &filePath)
{
    uint64_t hash = FNV1aOffset;
    std::ifstream in(filePath, std::ios::binary);

    if(!in) {
        throw std::runtime_error("Open failed: " + filePath.string());
    }

    std::vector<unsigned char> buf(m_chunkSize);
    in.rdbuf()->pubsetbuf((char*)buf.data(), buf.size());
    for (std::istreambuf_iterator<char> it(in), end; it != end; ++it) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(*it));
        hash *= FNV1aPrime;
    }

    return hash;
}
