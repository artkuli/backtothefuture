#include "scan.hpp"

#include "utils.hpp"

#include <iostream>
#include <unordered_map>


FsScanner::FsScanner(uint64_t maxFileSize) : m_maxFileSize(maxFileSize)
{

}

void FsScanner::scan(const std::filesystem::path &root, std::vector<DirEntry> &outputDirs, std::vector<FileEntry> &outputFiles, std::unordered_map<uint64_t, std::vector<size_t>>& filesBySize)
{
    if (!std::filesystem::exists(root) || !std::filesystem::is_directory(root)) {
        throw std::runtime_error("Invalid path: " + root.string());
    }

    std::error_code ec;
    for (auto it = std::filesystem::recursive_directory_iterator(
             root, std::filesystem::directory_options::follow_directory_symlink |
                 std::filesystem::directory_options::skip_permission_denied);
         it != std::filesystem::recursive_directory_iterator(); ++it)
    {
        const auto &path = it->path();
        if (it->is_directory(ec)) {
            DirEntry de;
            de.relativePath = std::filesystem::relative(path, root).generic_string();
            de.mtime = utils::filetime_to_time_t(std::filesystem::last_write_time(path));

            if (de.relativePath.empty())
                de.relativePath = ".";
            try {
                de.permissions = utils::perms_to_bits(std::filesystem::status(path).permissions());
            } catch (...) {
                de.permissions = 0;

            }
            outputDirs.push_back(std::move(de));
        } else if (it->is_regular_file(ec)) {
            FileEntry file;
            file.relativePath = std::filesystem::relative(path, root).generic_string();
            file.fullPath = path.string();
            file.mtime = utils::filetime_to_time_t(std::filesystem::last_write_time(path));
            if (file.relativePath.empty())
                continue;
            file.size = (uint64_t)std::filesystem::file_size(path, ec);
            if (ec)
                throw std::runtime_error("Cannot get size: " + path.string());

            if (file.size > m_maxFileSize)
                throw std::runtime_error("File exceeds 2GB: " + path.string());
            try {
                file.permissions = utils::perms_to_bits(std::filesystem::status(path).permissions());
            } catch (...) {
                file.permissions = 0;
            }
            outputFiles.push_back(std::move(file));
            filesBySize[file.size].push_back(outputFiles.size() - 1);
        }
        else {
            std::cerr << "Ignoring " << path.string() << " is not regular file or directory"<< std::endl;
        }
    }
}
