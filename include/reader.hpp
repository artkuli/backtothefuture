#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

class Reader {
public:
    uint32_t readU32(std::istream& stream);
    uint64_t readU64(std::istream& stream);
    std::string readString(std::istream& stream);
};
