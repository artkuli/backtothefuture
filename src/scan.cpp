#include "scan.hpp"
#include <iostream>

const u_int64_t kMaxFileSize = 2ull * 1024 * 1024 * 1024; // 2GB

uint32_t perms_to_bits(std::filesystem::perms p){
    using P = std::filesystem::perms;
    uint32_t b=0;
    auto on=[&](P bit,int pos){ if ((p & bit) != P::none) b|=(1u<<pos); };
    on(P::owner_read,0); on(P::owner_write,1); on(P::owner_exec,2);
    on(P::group_read,3); on(P::group_write,4); on(P::group_exec,5);
    on(P::others_read,6);on(P::others_write,7);on(P::others_exec,8);
    return b;
}

std::filesystem::perms bits_to_perms(uint32_t b){
    using P = std::filesystem::perms;
    auto on=[&](int pos,P bit){ return (b & (1u<<pos)) ? bit : P::none; };
    P p = P::none;
    p |= on(0,P::owner_read)|on(1,P::owner_write)|on(2,P::owner_exec);
    p |= on(3,P::group_read)|on(4,P::group_write)|on(5,P::group_exec);
    p |= on(6,P::others_read)|on(7,P::others_write)|on(8,P::others_exec);
    return p;
}

void FsScanner::scan(const std::filesystem::path &root, std::vector<DirEntry> &outputDirs, std::vector<ScannedFile> &outputFiles)
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
            if (de.relativePath.empty())
                de.relativePath = ".";
            try {
                de.permissions = perms_to_bits(std::filesystem::status(path).permissions());
            } catch (...) {
                de.permissions = 0;

            }
            outputDirs.push_back(std::move(de));
        } else if (it->is_regular_file(ec)) {
            ScannedFile file;
            file.relativePath = std::filesystem::relative(path, root).generic_string();
            if (file.relativePath.empty())
                continue;
            file.size = (uint64_t)std::filesystem::file_size(path, ec);
            if (ec)
                throw std::runtime_error("Cannot get size: " + path.string());

            if (file.size > kMaxFileSize)
                throw std::runtime_error("File exceeds 2GB: " + path.string());
            try {
                file.permissions = perms_to_bits(std::filesystem::status(path).permissions());
            } catch (...) {
                file.permissions = 0;
            }
            outputFiles.push_back(std::move(file));
        }
        else {
            std::cerr << "Ignoring " << path.string() << " is not regular file or directory"<< std::endl;
        }
    }
}
