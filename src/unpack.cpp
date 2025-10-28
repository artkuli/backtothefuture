#include "unpack.hpp"

#include "types.hpp"
#include "unarchiver.hpp"
#include "archivereader.hpp"

#include <iostream>

int unpackCmd(const Config& config, const std::filesystem::path& archive_path, const std::filesystem::path& output_dir)
{
    if (!std::filesystem::exists(archive_path)) {
        std::cerr << "The archive file does not exist: " << archive_path << std::endl;
        return 1;
    }
    if (!std::filesystem::is_regular_file(archive_path)) {
        std::cerr << "The archive path is not a regular file: " << archive_path << std::endl;
        return 1;
    }
    if (!std::filesystem::exists(output_dir)) {
        std::cout << "Output directory doesn't exists, trying to create: " << output_dir << std::endl;
        if (!std::filesystem::create_directory(output_dir)) {
            std::cerr << "Cannot create a directory: " << output_dir << std::endl;
            return 1;
        }
    }

    try {
        BinaryArchiveReader reader{archive_path};
        if (!reader.isOpen()) {
            std::cerr << "An error occurred while trying to open the archive file: " << archive_path << std::endl;
            return 1;
        }

        const Header hdr = reader.readHeader();
        std::cout << "Archive version: " << hdr.version
                  << ", files to extract: " << hdr.fileCount
                  << ", dirs to extract: " << hdr.dirCount << std::endl;

        Unarchiver unarchiver(reader, config);
        unarchiver.unarchive(output_dir);

        std::cout << "Unarchiving to the directory: " << output_dir << " completed successfully" << std::endl;
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
        return 1;
    }
}
