#include "test_utils.hpp"

#include <fstream>

std::string bytes(std::initializer_list<uint8_t> b) {
    std::string s;
    s.reserve(b.size());
    for (auto v : b) {
        s.push_back(static_cast<char>(v));
    }
    return s;
}

std::filesystem::__cxx11::path makeTempDir(const std::string &stem) {
    auto base = std::filesystem::temp_directory_path()
                / (stem + "-" + std::to_string(std::rand()));
    std::filesystem::create_directories(base);
    return base;
}

TempFile::TempFile(const std::string &stem, const std::vector<uint8_t> &data) {
    auto dir = makeTempDir(stem);
    path = dir / (stem + "-" + std::to_string(std::rand()) + ".bin");
    std::ofstream out(path, std::ios::binary);
    out.write(reinterpret_cast<const char*>(data.data()),
              static_cast<std::streamsize>(data.size()));
    out.close();
}

TempFile::~TempFile() {
    std::error_code ec;
    std::filesystem::remove(path, ec);
}
