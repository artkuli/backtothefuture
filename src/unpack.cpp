#include "unpack.hpp"

#include <iostream>
#include <vector>
#include "reader.hpp"
#include "types.hpp"
#include "utils.hpp"

namespace Unpackager
{

struct BlobRecord {
    uint64_t size = 0;
    uint64_t hash64 = 0;
    uint64_t dataOffset = 0;
};

void extractAll(const std::vector<DirEntry>& dirs,
           const std::vector<FileEntry>& files,
           const std::vector<BlobRecord> &blobRecords,
           const std::filesystem::path& targetRoot,
           const std::filesystem::path& inputPath)
{
    for(const auto& d: dirs){
        auto outDir = targetRoot / std::filesystem::path(d.relativePath);
        std::error_code ec;
        std::filesystem::create_directories(outDir, ec);
        if(!ec){
            std::filesystem::permissions(outDir,
                                         utils::bits_to_perms(d.permissions),
                                         std::filesystem::perm_options::replace, ec);
        }
    }
    std::ifstream is(inputPath, std::ios::binary);
    if(!is) throw std::runtime_error("Nie można ponownie otworzyć archiwum");
    for(const auto& f: files){
        const auto& b = blobRecords.at(size_t(f.blobId));
        auto outPath = targetRoot / std::filesystem::path(f.relativePath).make_preferred();
        std::filesystem::create_directories(outPath.parent_path());
        is.clear();
        is.seekg(std::streamoff(b.dataOffset), std::ios::beg);
        if(!is) throw std::runtime_error("seekg(blob) nieudany");

        std::ofstream os(outPath, std::ios::binary | std::ios::trunc);
        if(!os) throw std::runtime_error("Nie można utworzyć pliku: "+outPath.string());
        // Zrzucamy dokładnie f.size bajtów (nawet jeśli blob jest współdzielony)
        const size_t CHUNK = 1<<20;
        std::vector<char> buf(CHUNK);
        uint64_t left = f.size;
        while(left){
            size_t take = size_t(std::min<uint64_t>(left, CHUNK));
            is.read(buf.data(), std::streamsize(take));
            if(!is) throw std::runtime_error("Błąd odczytu danych pliku");
            os.write(buf.data(), std::streamsize(take));
            if(!os) throw std::runtime_error("Błąd zapisu pliku");
            left -= take;
        }

        std::error_code ec;
        std::filesystem::permissions(outPath,
                                     utils::bits_to_perms(f.permissions),
                                     std::filesystem::perm_options::replace, ec);
        (void)ec;
    }
}


int unpack(const std::filesystem::path& archive_path, const std::filesystem::path& output_dir)
{
    if (!std::filesystem::exists(archive_path)) {
        std::cerr << "The archive file does not exist: " << archive_path << std::endl;
        return 1;
    }

    if (!std::filesystem::is_regular_file(archive_path))
    {
        std::cerr << "The archive path is not a regular file: " << archive_path << std::endl;
        return 1;
    }

    if (!std::filesystem::exists(output_dir) || !std::filesystem::is_directory(output_dir))
    {
        std::cerr << "Invalid output directory: " << output_dir << std::endl;
        return 1;
    }

    Reader reader{archive_path};
    if (!reader.isOpen()) {
        std::cerr << "An error occurred while trying to open the archive file: " << archive_path << std::endl;
        return 1;
    }

    auto header = reader.readHeader();
    std::cout << "Archive version: " << header.version
              << ", files to extract: " << header.fileCount
              << ", dirs to extract" << header.dirCount << std::endl;

    auto& inStream = reader.stream();
    inStream.seekg(std::streamoff(header.dirTableOff), std::ios::beg);

    std::vector<DirEntry> dirs;
    dirs.reserve(size_t(header.dirCount));
    // read dirs
    for(uint64_t i = 0; i < header.dirCount; ++i) {
        DirEntry d;
        d.relativePath = reader.read_string(inStream);
        d.permissions  = reader.read_u32(inStream);
        d.mtime = reader.read_u64(inStream);
        dirs.push_back(std::move(d));
    }
    // read file list
    std::vector<FileEntry> files;
    inStream.seekg(std::streamoff(header.fileTableOff), std::ios::beg);
    files.reserve(size_t(header.fileCount));
    for(uint64_t i = 0; i < header.fileCount; ++i) {
        FileEntry f;
        f.relativePath = reader.read_string(inStream);
        f.permissions  = reader.read_u32(inStream);
        f.size         = reader.read_u64(inStream);
        f.mtime        = reader.read_u64(inStream);
        f.blobId       = reader.read_u64(inStream);

        if(f.blobId >= header.blobCount) {
            throw std::runtime_error("Uszkodzony blobId");
        }
        files.push_back(std::move(f));
    }

    // read blobTable
    std::vector<BlobRecord> blobRecords;
    inStream.seekg(std::streamoff(header.blobTableOff), std::ios::beg);
    blobRecords.reserve(size_t(header.blobCount));
    for(uint64_t i=0;i<header.blobCount;++i) {
        BlobRecord blobRecord;
        blobRecord.size = reader.read_u64(inStream);
        blobRecord.hash64 = reader.read_u64(inStream);
        blobRecord.dataOffset = reader.read_u64(inStream);
        if(blobRecord.dataOffset == 0 && blobRecord.size > 0) {
            throw std::runtime_error("Pusty dataOffset blobu");
        }
        blobRecords.push_back(std::move(blobRecord));
    }

    extractAll(dirs, files, blobRecords, output_dir, archive_path);

    return 0;
}

}
