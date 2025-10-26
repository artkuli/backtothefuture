#include "duplicateresolver.h"

#include <fstream>
#include <vector>
#include <future>

constexpr uint64_t kNoHashSentinel = 0;

struct BlobInfo {
    size_t fileIndex = 0;
    uint64_t hash = 0;
    std::filesystem::path source;
};


DuplicateResolver::DuplicateResolver(IHasher &hasher, IFilesComparer &cfilesComparer)
    : m_hasher(hasher)
    , m_filesComparer(cfilesComparer)
{

}

void DuplicateResolver::resolve(const std::filesystem::path& inputDir, std::vector<FileEntry> &files, const std::unordered_map<uint64_t, std::vector<size_t> > &filesBySize)
{
    for (auto& keyValue : filesBySize) {
        auto &indices = keyValue.second;
        if (indices.size() == 1) {

            files[indices[0]].hash = kNoHashSentinel;
            continue;
        }

        std::vector<std::future<std::pair<uint64_t, size_t>>> futures;
        futures.reserve(indices.size());
        for (auto &idx : indices) {
            futures.push_back(std::async(std::launch::async, [&, idx]() -> std::pair<uint64_t, size_t> {
                auto& entry = files[idx];
                FNV1aHasher hasher;
                entry.hash = hasher.hash_file(entry.fullPath);
                return {entry.hash, idx};
            }));
        }
        std::unordered_map<uint64_t, std::vector<size_t>> hashToFileEntries;
        hashToFileEntries.reserve(indices.size());

        for (auto& future : futures) {
            auto res = future.get();
            hashToFileEntries[res.first].push_back(res.second);
        }

        std::unordered_map<uint64_t, BlobInfo> hashToBlobInfo;
        for (auto& sameHashFiles : hashToFileEntries) {
            auto& fileEntryIndices = sameHashFiles.second;
            if (fileEntryIndices.size() != 1) {
                for (auto idx : fileEntryIndices) {
                    auto &file = files[idx];
                    auto it = hashToBlobInfo.find(file.hash);
                    if (it != hashToBlobInfo.end())
                    {
                        if (m_filesComparer.areEqual(inputDir / file.relativePath,
                                                     inputDir / it->second.source,
                                                   8 << 20)) {
                            file.source_example = it->second.source;
                            break;
                        }
                    }
                    hashToBlobInfo[file.hash] = BlobInfo{idx, file.hash, file.relativePath};
                }
            }
        }
    }

}
