#include "pack.hpp"

#include "archiver.hpp"
#include "scan.hpp"
#include "hasher.hpp"
#include "filescomparer.hpp"
#include "duplicateresolver.hpp"
#include "archivewriter.hpp"
#include "config.hpp"

#include <iostream>

#include <vector>
#include <cstring>

int packCmd(const Config& config, const std::filesystem::path &inputDir, const std::filesystem::path &archivePath)
{
    if (!std::filesystem::exists(inputDir) || !std::filesystem::is_directory(inputDir)) {
        std::cerr << "Invalid path to the input directory: " << inputDir.string() << std::endl;
        return 1;
    }

    if (!archivePath.has_parent_path()) {
        if (!std::filesystem::create_directories(archivePath.parent_path())) {
            std::cerr << "An error occurred while trying to create directories for: " << archivePath.parent_path() << std::endl;
            return 1;
        }
    }

    std::vector<FileInfo> files;
    std::vector<DirInfo> dirs;
    std::vector<BlobInfo> blobs;
    std::unordered_map<uint64_t, std::vector<size_t>> filesBySize;

    std::cout << "Starting scanning directory: " << inputDir << std::endl;

    FsScanner scanner(config.maxFileSize);
    scanner.scan(inputDir, dirs, files, filesBySize);

    if (files.size() > config.maxFilesCount) {
        std::cerr << "The number of files exceeds the maximum allowed: " << files.size() << " > " << config.maxFilesCount << std::endl;
        return 1;
    }
    std::cout << "Scanned " << files.size() << " files and " << dirs.size() << " directories" << std::endl;


    std::cout << "Starting resolving duplicates..." << std::endl;
    DefaultFileComparer filesComparer;
    DuplicateResolver duplicateResolver(config, filesComparer);
    duplicateResolver.resolve(inputDir, files, filesBySize, blobs);

    std::cout << "Detected " << blobs.size() << " unique blobs from " << files.size() << " files" << std::endl;
    std::cout << "Starting archiving to " << archivePath << std::endl;

    BinaryArchiveWriter archiveWriter(archivePath, config.chunkSize);
    Archiver archiver(archiveWriter);
    archiver.archive(dirs, files, blobs, inputDir);

    std::cout << "Archiving to file: " << archivePath.string() << " completed successfully" << std::endl;
    return 0;
}
