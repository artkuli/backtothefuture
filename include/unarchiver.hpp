#pragma once

#include <filesystem>
#include "archivereader.hpp"


struct Config;
class IArchiveReader;

class Unarchiver
{
public:
    explicit Unarchiver(IArchiveReader& reader, const Config& config);
    void unarchive(const std::filesystem::path& outputDir);
private:
    IArchiveReader& m_archiveReader;
    const Config& m_config;
};
