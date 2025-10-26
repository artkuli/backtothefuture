#include "archiver.hpp"


#include "types.hpp"
#include "writer.hpp"
#include <iostream>

/*
*   [header files_count=.. dirs_count=.. ]
*   [dir entries = path, mtime, perms]
*   [file entries, path, size, mtime, perms, source]
*   [blobs files data ]
*/

bool Archiver::archive(const std::vector<DirEntry> &dirs,
                       const std::vector<FileEntry> &files,
                       const std::vector<BlobInfo> &blobs,
                       const std::filesystem::path &inputDir,
                       const std::filesystem::path &archivePath)
{
    if (!archivePath.has_parent_path()) {
        if (!std::filesystem::create_directories(archivePath.parent_path())) {
            std::cerr << "An error occurred while trying to create directories for: " << archivePath.parent_path() << std::endl;
            return false;
        }
    }

    Writer writer(archivePath);
    if (!writer.isOpen()) {
        std::cerr << "An error occurred while tried to open file: " << archivePath << std::endl;
        return false;
    }
    auto headerPos = writer.currentPos();
    writer.writeHeader(dirs.size(), files.size(), blobs.size());

    auto dirTableOffset = writer.currentPos();
    writer.writeDirs(dirs);

    auto fileTableOffset = writer.currentPos();
    writer.writeFiles(files);

    auto blobTableOffset = writer.currentPos();
    writer.writeBlobs(inputDir, blobs);

    writer.updateOffsets(headerPos, dirTableOffset, fileTableOffset, blobTableOffset);
    return true;
}
