#include "duplicateresolver.hpp"

#include "config.hpp"

#include <vector>
#include <future>
#include <optional>

constexpr uint64_t kNoHashSentinel = 0;

DuplicateResolver::DuplicateResolver(const Config& config, IFilesComparer &cfilesComparer)
    : m_config(config)
    , m_filesComparer(cfilesComparer)
{

}

void DuplicateResolver::resolve(const std::filesystem::path& inputDir, std::vector<FileInfo> &files, const std::unordered_map<uint64_t, std::vector<size_t> > &filesBySize, std::vector<BlobInfo>& blobs)
{
    auto makeBlobInfo = [&](const FileInfo& f, std::optional<uint64_t> h = std::nullopt) -> size_t {
        BlobInfo info{};
        info.size   = f.size;
        info.source = f.relativePath;
        if (h) {
            info.hash64 = *h;
        }
        blobs.push_back(std::move(info));
        return blobs.size() - 1;
    };

    for (auto& keyValue : filesBySize) {
        auto &indices = keyValue.second;
        if (indices.size() == 1)
        {
            auto& file = files[indices[0]];
            file.blobId = makeBlobInfo(file);
            continue;
        }

        std::vector<std::future<std::pair<uint64_t, size_t>>> futures;
        futures.reserve(indices.size());
        for (auto &idx : indices) {
            futures.push_back(std::async(std::launch::async, [&, idx]() -> std::pair<uint64_t, size_t> {
                auto& entry = files[idx];
                FNV1aHasher hasher(m_config.chunkSize);
                return {hasher.hash_file(entry.fullPath), idx};
            }));
        }
        std::unordered_map<uint64_t, std::vector<size_t>> hashToFileEntries;
        hashToFileEntries.reserve(indices.size());

        for (auto& future : futures) {
            auto res = future.get();
            hashToFileEntries[res.first].push_back(res.second);
        }

        for (auto& sameHashFiles : hashToFileEntries) {
            auto& FileInfoIndices = sameHashFiles.second;
            std::vector<FileInfo*> processedFiles;
            for (int idx = 0; idx < FileInfoIndices.size(); ++idx)
            {
                auto& file = files[FileInfoIndices[idx]];
                if (idx == 0)
                {
                    file.blobId = makeBlobInfo(file, sameHashFiles.first);
                    processedFiles.push_back(&file);
                }
                else
                {
                    bool matched = false;
                    for (auto* processedFile : processedFiles)
                    {
                        if (m_filesComparer.areEqual(inputDir / file.relativePath,
                                                  inputDir / processedFile->relativePath,
                                                  m_config.chunkSize))
                        {
                            file.blobId = processedFile->blobId;;
                            matched = true;
                            break;
                        }
                    }
                    if (!matched)
                    {
                        // unique file, the same hash but never meet a equal file
                        file.blobId = makeBlobInfo(file, sameHashFiles.first);
                        processedFiles.push_back(&file);
                    }

                }
            }
        }
    }
}
