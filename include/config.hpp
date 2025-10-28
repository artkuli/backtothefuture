#pragma once

#include <stddef.h>
#include <stdint.h>

struct Config {
    size_t chunkSize = 8 << 20; // 8MB
    uint64_t maxFileSize = 2ull * 1024 * 1024 * 1024; // 2GB
    uint32_t maxFilesCount = 1048576;
};
