// fnv1a_hasher_gtest.cpp
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <string>

#include "hasher.hpp"

class FNV1aHasherTest : public ::testing::Test {
protected:
    static std::unique_ptr<FNV1aHasher> hasher;

    static void SetUpTestSuite() {
        hasher = std::make_unique<FNV1aHasher>(8 << 20);
    }

    static void TearDownTestSuite() {
        hasher.reset();
    }
};

std::unique_ptr<FNV1aHasher> FNV1aHasherTest::hasher;


static std::filesystem::path WriteTempFile(const std::vector<unsigned char>& data,
                                           const std::string& name_hint = "fnv1a_test") {
    const auto dir = std::filesystem::temp_directory_path();
    auto path = dir / std::filesystem::path(name_hint + "_" + std::to_string(::getpid()) + "_" + std::to_string(std::rand()));
    std::ofstream out(path, std::ios::binary);
    if (!out) throw std::runtime_error("Cannot create temp file: " + path.string());
    if (!data.empty()) out.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
    out.close();
    return path;
}

TEST_F(FNV1aHasherTest, EmptyFileReturnsOffsetBasis) {
    std::vector<unsigned char> data;
    auto p = WriteTempFile(data, "empty");
    EXPECT_EQ(hasher->hash_file(p), 0xcbf29ce484222325ULL);
    std::filesystem::remove(p);
}

TEST_F(FNV1aHasherTest, KnownVectorsSmallText) {
    {
        std::vector<unsigned char> data{'a'};
        auto p = WriteTempFile(data, "a");
        EXPECT_EQ(hasher->hash_file(p), 0xAF63DC4C8601EC8CULL);
        std::filesystem::remove(p);
    }
    {
        std::vector<unsigned char> data{'a','b','c'};
        auto p = WriteTempFile(data, "abc");
        EXPECT_EQ(hasher->hash_file(p), 0xE71FA2190541574BULL);
        std::filesystem::remove(p);
    }
}

TEST_F(FNV1aHasherTest, FullByteRange256) {
    std::vector<unsigned char> data(256);
    for (size_t i = 0; i < 256; ++i) data[i] = static_cast<unsigned char>(i);
    auto p = WriteTempFile(data, "range256");
    EXPECT_EQ(hasher->hash_file(p), 0x4242DC5249C33625ULL);
    std::filesystem::remove(p);
}

TEST_F(FNV1aHasherTest, LargerThanInternalBuffer) {
    const size_t N = 9 * 1024 * 1024;
    std::vector<unsigned char> data(N);
    for (size_t i = 0; i < N; ++i) data[i] = static_cast<unsigned char>(i & 0xFF);
    auto p = WriteTempFile(data, "big_9MiB");

    EXPECT_EQ(hasher->hash_file(p), 0x1BC4E941BED22325ULL); // Replace with actual expected hash);

    std::filesystem::remove(p);
}

TEST_F(FNV1aHasherTest, ThrowsOnMissingFile) {
    auto p = std::filesystem::temp_directory_path() / "definitely_missing_fnv1a_test.bin";
    ASSERT_FALSE(std::filesystem::exists(p));
    try {
        hasher->hash_file(p);
        FAIL() << "Expected std::runtime_error";
    } catch (const std::runtime_error& e) {
        std::string what = e.what();
        EXPECT_NE(what.find("open failed"), std::string::npos) << what;
        EXPECT_NE(what.find(p.string()), std::string::npos) << what;
    } catch (...) {
        FAIL() << "Expected std::runtime_error";
    }
}

TEST_F(FNV1aHasherTest, DeterministicAcrossRuns) {
    std::vector<unsigned char> data = {'T','e','s','t','!','\n',0x00,0x7F};
    auto p = WriteTempFile(data, "determinism");
    const auto h1 = hasher->hash_file(p);
    const auto h2 = hasher->hash_file(p);
    EXPECT_EQ(h1, h2);
    std::filesystem::remove(p);
}
