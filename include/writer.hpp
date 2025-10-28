#pragma once

#include <fstream>
#include <cstdint>

class Writer
{
public:
    void writeU32(uint32_t v, std::ostream& stream);
    void writeU64(uint64_t v, std::ostream& stream);
    void writeString(const std::string &s, std::ostream& stream);
    void copyBytes(std::ifstream& in, uint64_t n, std::ostream& out, size_t chunkSize);
};
