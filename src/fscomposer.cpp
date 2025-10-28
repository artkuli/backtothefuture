#include "fscomposer.hpp"

#include "types.hpp"

std::string errorWithPath(const std::string& msg, const std::filesystem::path& p) {
    return msg + ": " + p.string();
}

FSComposer::FSComposer(const std::size_t chunkSize)
    : m_chunkSize(chunkSize)
{
}

void FSComposer::composeDirectories(const std::vector<DirInfo> &dirs, const std::filesystem::path &root) const
{
    if (dirs.empty()) {
        return;
    }

    for (const auto& d : dirs) {
        const auto outDir = resolvePath(root, d.relativePath);
        std::error_code ec;
        std::filesystem::create_directories(outDir, ec);
        if (ec)
        {
            throw std::runtime_error(errorWithPath("Not able to create directory", outDir));
        }

        applyPermissions(outDir, d.permissions);
        applyMtime(outDir, static_cast<std::int64_t>(d.mtime));
    }
}

void FSComposer::composeFiles(IArchiveReader& archive,
                            const std::vector<FileInfo>& files,
                            const std::vector<BlobRecord>& blobs,
                            const std::filesystem::path& rootDir) const {
    if (files.empty()) {
        return;
    }

    std::vector<char> buf(m_chunkSize);

    for (const auto& f : files) {
        if (static_cast<size_t>(f.blobId) >= blobs.size()) {
            throw std::runtime_error("Corrupted file, invalid blob identifier");
        }

        const auto& b = blobs[static_cast<size_t>(f.blobId)];
        const auto outPath = resolvePath(rootDir, f.relativePath);

        if (!std::filesystem::exists(outPath.parent_path())) {
            throw std::runtime_error(errorWithPath("Parent directory does not exist for file", outPath));
        }

        auto in = archive.openBlobStream(b.dataOffset);
        if (!in) {
            throw std::runtime_error(errorWithPath("Cannot open blob stream for file", outPath));
        }

        std::ofstream os(outPath, std::ios::binary | std::ios::trunc);
        if (!os) {
            throw std::runtime_error(errorWithPath("Cannot open the file", outPath));
        }

        uint64_t left = f.size;
        while (left > 0) {
            const std::size_t take = static_cast<std::size_t>(std::min<uint64_t>(left, buf.size()));
            if (take == 0) {
                break;
            }

            in->read(buf.data(), static_cast<std::streamsize>(take));
            if (!in) {
                throw std::runtime_error(errorWithPath("An error occurred while reading the blob for", outPath));
            }

            os.write(buf.data(), static_cast<std::streamsize>(take));
            if (!os) {
                throw std::runtime_error(errorWithPath("An error occurred while writing to file", outPath));
            }
            left -= take;
        }
        os.close();
        if (!os) {
            throw std::runtime_error(errorWithPath("Flush/close failed for file", outPath));
        }

        applyPermissions(outPath, f.permissions);
        applyMtime(outPath, static_cast<std::int64_t>(f.mtime));
    }
}

std::filesystem::path FSComposer::resolvePath(const std::filesystem::path &root, const std::string &relative)
{
    const auto targetPath = std::filesystem::path(root / relative).make_preferred();
    const auto canonicalRoot = std::filesystem::canonical(root);
    const auto canonicalTarget = std::filesystem::weakly_canonical(targetPath);
    if (canonicalTarget.string().find(canonicalRoot.string()) != 0) {
        throw std::runtime_error(errorWithPath("Resolved path is outside of the root directory: ", targetPath));
    }
    return canonicalTarget;
}

void FSComposer::applyPermissions(const std::filesystem::path &p, uint32_t mode) const
{
    std::error_code ec;
    std::filesystem::permissions(p, utils::bits_to_perms(mode),
                                 std::filesystem::perm_options::replace, ec);
    if (ec) {
        throw std::runtime_error(errorWithPath("Not able to set permissions", p));
    }
}

void FSComposer::applyMtime(const std::filesystem::path &p, uint64_t mtime) const
{
    std::error_code ec;
    std::filesystem::last_write_time(p, utils::time_t_to_filetime(mtime), ec);
    if (ec) {
        throw std::runtime_error(errorWithPath("Not able to set modification time", p));
    }
}
