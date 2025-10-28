#pragma once

#include <cstdio>
#include <filesystem>

struct IHasher {
    virtual ~IHasher() = default;
    virtual uint64_t hash_file(const std::filesystem::path& p) = 0;
};

struct FNV1aHasher : IHasher {
    FNV1aHasher(size_t chunkSize);
    uint64_t hash_file(const std::filesystem::path& p) override;
private:
    size_t m_chunkSize;
};
