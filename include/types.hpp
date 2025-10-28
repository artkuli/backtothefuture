#pragma once

#include <cstdint>
#include <string>
#include <filesystem>

struct DirInfo {
    std::string relativePath;
    unsigned int permissions = 0;
    uint64_t mtime = 0;
};

struct BlobInfo {
    uint64_t size = 0;
    uint64_t hash64 = 0;
    std::filesystem::path source;
};

struct FileInfo {
    std::filesystem::path relativePath;
    std::filesystem::path fullPath;
    uint32_t permissions = 0;
    uint64_t size = 0;
    uint64_t mtime = 0;
    uint64_t blobId;
};

struct Header {
    uint32_t version = 1;
    uint64_t dirCount = 0;
    uint64_t fileCount = 0;
    uint64_t blobCount = 0;
    uint64_t dirTableOffset = 0;
    uint64_t fileTableOffset = 0;
    uint64_t blobTableOffset = 0;
    uint64_t blobDataOffset = 0;
};
