#include "unarchiver.hpp"

#include "types.hpp"
#include "config.hpp"
#include "archivereader.hpp"
#include "fscomposer.hpp"

Unarchiver::Unarchiver(IArchiveReader &reader, const Config &config)
    : m_archiveReader(reader)
    , m_config(config)
{

}

void Unarchiver::unarchive(const std::filesystem::path &outputDir)
{
    if (!std::filesystem::exists(outputDir) || !std::filesystem::is_directory(outputDir)) {
        throw std::runtime_error("Invalid output directory: " + outputDir.string());
    }

    if (!m_archiveReader.isOpen()) {
        throw std::runtime_error("Cannot open archive");
    }

    const Header header = m_archiveReader.readHeader();
    const auto dirs = m_archiveReader.readDirs(header);
    const auto files = m_archiveReader.readFiles(header);
    const auto blobs = m_archiveReader.readBlobs(header);
    FSComposer composer(m_config.chunkSize);

    composer.composeDirectories(dirs, outputDir);
    composer.composeFiles(m_archiveReader, files, blobs, outputDir);
}


