#include "scan.hpp"
#include "utils.hpp"

#include "test_utils.hpp"

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

static std::filesystem::path writeFile(const std::filesystem::path& dir,
                                       const std::string& rel,
                                       const std::string& content) {
    auto p = dir / rel;
    std::filesystem::create_directories(p.parent_path());
    std::ofstream out(p, std::ios::binary);
    out.write(content.data(), static_cast<std::streamsize>(content.size()));
    out.close();
    return p;
}

static bool hasDirRel(const std::vector<DirInfo>& dirs, const std::string& rel) {
    for (const auto& d : dirs) if (d.relativePath == rel) return true;
    return false;
}

TEST(FsScannerTest, ScansDirsAndFilesAndGroupsBySize) {
    auto root = makeTempDir("fsscaner-ScansDirsAndFilesAndGroupsBySize");
    auto pA = writeFile(root, "a.txt", "abc");        // 3
    auto pB = writeFile(root, "d1/b.txt", "xyz");     // 3
    auto pC = writeFile(root, "d1/c.bin", "12345");   // 5

    FsScanner scanner(1024 * 1024);
    std::vector<DirInfo> outDirs;
    std::vector<FileInfo> outFiles;
    std::unordered_map<uint64_t, std::vector<size_t>> filesBySize;

    ASSERT_NO_THROW(scanner.scan(root, outDirs, outFiles, filesBySize));

    EXPECT_TRUE(hasDirRel(outDirs, "d1"));

    ASSERT_EQ(outFiles.size(), 3u);

    ASSERT_TRUE(filesBySize.count(3u) > 0);
    ASSERT_TRUE(filesBySize.count(5u) > 0);
    EXPECT_EQ(filesBySize[3u].size(), 2u);
    EXPECT_EQ(filesBySize[5u].size(), 1u);

    std::unordered_map<std::string, std::pair<std::filesystem::path, uint64_t>> expected = {
                                                                                            {"a.txt", {pA, 3u}},
                                                                                            {"d1/b.txt", {pB, 3u}},
                                                                                            {"d1/c.bin", {pC, 5u}},
                                                                                            };

    for (size_t i = 0; i < outFiles.size(); ++i) {
        const auto& f = outFiles[i];
        SCOPED_TRACE(::testing::Message() << "file idx=" << i << " rel=" << f.relativePath);

        ASSERT_TRUE(expected.count(f.relativePath) > 0);
        const auto& exp = expected[f.relativePath];

        EXPECT_EQ(std::filesystem::path(f.fullPath), exp.first);
        EXPECT_EQ(f.size, exp.second);

        auto permsBits = utils::perms_to_bits(std::filesystem::status(exp.first).permissions());
        EXPECT_EQ(f.permissions, permsBits);

        EXPECT_GE(f.mtime, 0);

        auto it = filesBySize.find(f.size);
        ASSERT_TRUE(it != filesBySize.end());
        const auto& idxs = it->second;
        EXPECT_NE(std::find(idxs.begin(), idxs.end(), i), idxs.end());
    }
}

TEST(FsScannerTest, ThrowsWhenFileExceedsMaxFileSize) {
    auto root = makeTempDir("fsscaner-ThrowsWhenFileExceedsMaxFileSize");

    writeFile(root, "big.bin", "12345");

    FsScanner scanner(4);
    std::vector<DirInfo> outDirs;
    std::vector<FileInfo> outFiles;
    std::unordered_map<uint64_t, std::vector<size_t>> filesBySize;

    EXPECT_THROW(scanner.scan(root, outDirs, outFiles, filesBySize), std::runtime_error);
}
