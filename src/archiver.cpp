#include "archiver.hpp"


#include "types.hpp"
#include "writer.hpp"
#include "archivewriter.hpp"
#include <iostream>

Archiver::Archiver(IArchiveWriter &writer)
    : m_archiveWriter(writer)
{}

bool Archiver::archive(const std::vector<DirInfo> &dirs,
                       const std::vector<FileInfo> &files,
                       const std::vector<BlobInfo> &blobs,
                       const std::filesystem::path &inputDir)
{
    if (!m_archiveWriter.isOpen()) {
        std::cerr << "An error occurred while tried to open file" << std::endl;
        return false;
    }
    auto headerPos = m_archiveWriter.currentPos();
    Header header;
    header.dirCount = dirs.size();
    header.fileCount = files.size();
    header.blobCount = blobs.size();
    m_archiveWriter.writeHeader(header, headerPos);

    auto dirTableOffset = m_archiveWriter.currentPos();
    m_archiveWriter.writeDirs(dirs);

    auto fileTableOffset = m_archiveWriter.currentPos();
    m_archiveWriter.writeFiles(files);

    auto blobTableOffset = m_archiveWriter.currentPos();
    auto blobOffsetPlaceHolders = m_archiveWriter.writeBlobsTable(blobs);

    auto blobDataOffset = m_archiveWriter.currentPos();
    m_archiveWriter.writeBlobs(inputDir, blobs, blobOffsetPlaceHolders);

    header.dirTableOffset = dirTableOffset;
    header.fileTableOffset = fileTableOffset;
    header.blobTableOffset = blobTableOffset;
    header.blobDataOffset = blobDataOffset;

    // update offsets in header
    m_archiveWriter.writeHeader(header, headerPos);

    return true;
}
