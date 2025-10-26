#include "duplicateresolver.hpp"

#include <fstream>
#include <vector>
#include <future>
#include <iostream>

constexpr uint64_t kNoHashSentinel = 0;

DuplicateResolver::DuplicateResolver(IHasher &hasher, IFilesComparer &cfilesComparer)
    : m_hasher(hasher)
    , m_filesComparer(cfilesComparer)
{

}

void DuplicateResolver::resolve(const std::filesystem::path& inputDir, std::vector<FileEntry> &files, const std::unordered_map<uint64_t, std::vector<size_t> > &filesBySize, std::vector<BlobInfo>& blobs)
{
    for (auto& keyValue : filesBySize) {
        auto &indices = keyValue.second;
        if (indices.size() == 1)
        {
            auto& file = files[indices[0]];
            BlobInfo info;
            info.size = file.size;
            info.source = file.relativePath;
            blobs.push_back(info);
            file.blobId = blobs.size() - 1;
            continue;
        }

        std::vector<std::future<std::pair<uint64_t, size_t>>> futures;
        futures.reserve(indices.size());
        for (auto &idx : indices) {
            futures.push_back(std::async(std::launch::async, [&, idx]() -> std::pair<uint64_t, size_t> {
                auto& entry = files[idx];
                FNV1aHasher hasher;               
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
            auto& fileEntryIndices = sameHashFiles.second;
            std::vector<FileEntry*> processedFiles;
            for (int idx = 0; idx < fileEntryIndices.size(); ++idx)
            {
                auto& file = files[fileEntryIndices[idx]];
                if (idx == 0)
                {
                    BlobInfo info;
                    info.hash64 = sameHashFiles.first;
                    info.size = file.size;
                    info.source = file.relativePath;
                    blobs.push_back(info);
                    file.blobId = blobs.size() - 1;
                    processedFiles.push_back(&file);
                }
                else
                {
                    bool matched = false;
                    for (auto* processedFile : processedFiles)
                    {
                        if (m_filesComparer.areEqual(inputDir / file.relativePath,
                                                     inputDir / processedFile->relativePath,
                                                   8 << 20))
                        {
                            file.blobId = processedFile->blobId;;
                            std::cout << "File" << file.fullPath.string() << " Equal with=" << processedFile->fullPath.string() << std::endl;
                            matched = true;
                            break;
                        }
                    }
                    if (!matched)
                    {
                        // unique file, the same hash but never meet a equal file
                        BlobInfo info;
                        info.hash64 = sameHashFiles.first;
                        info.size = file.size;
                        info.source = file.relativePath;
                        blobs.push_back(info);
                        file.blobId = blobs.size() - 1;
                        processedFiles.push_back(&file);
                    }

                }
            }
        }
    }

}
