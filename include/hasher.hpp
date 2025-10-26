#pragma once

#include <cstdio>
#include <filesystem>

struct IHasher {
    virtual ~IHasher() = default;
    virtual uint64_t hash_file(const std::filesystem::path& p) = 0;
};

struct FNV1aHasher : IHasher {
    uint64_t hash_file(const std::filesystem::path& p) override;
};
