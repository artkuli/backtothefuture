#pragma once

#include <filesystem>

class IFilesComparer
{
public:
    virtual bool areEqual(const std::filesystem::path& path1, const std::filesystem::path& path2, size_t buf_bytes) = 0;
};


class DefaultFileComparer final : public IFilesComparer
{
public:
    bool areEqual(const std::filesystem::path &path1, const std::filesystem::path &path2, size_t buf_bytes) override;
};
