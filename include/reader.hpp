#pragma once

#include <cstdint>
#include <fstream>
#include <filesystem>

struct Header;

class Reader {
public:
    Reader(const std::filesystem::path& inputPath);
    bool isOpen() const;
    std::streampos currentPos();;

    Header readHeader();
    std::ifstream& stream();

    uint32_t read_u32(std::ifstream &stream);
    uint64_t read_u64(std::ifstream &stream);
    std::string read_string(std::ifstream &stream);

private:
    std::ifstream m_in;


};
