#pragma once

#include <string>

struct DirEntry {
    std::string relativePath;
    unsigned int permissions = 0;
};
