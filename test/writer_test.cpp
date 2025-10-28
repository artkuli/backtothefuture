#include "writer.hpp"

#include "test_utils.hpp"
#include <gtest/gtest.h>
#include <cstdint>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

using namespace std;

TEST(WriterTest, WriteU64LittleEndianBasic) {
    Writer w;
    std::ostringstream oss(std::ios::binary);

    uint64_t v = 0x0102030405060708ULL;
    w.writeU64(v, oss);

    std::string got = oss.str();
    ASSERT_EQ(got.size(), 8u);
    EXPECT_EQ(got, bytes({0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01}));
}

TEST(WriterTest, WriteU64ZeroAndMax) {
    Writer w;
    std::ostringstream oss0(std::ios::binary), ossMax(std::ios::binary);

    w.writeU64(0ULL, oss0);
    w.writeU64(std::numeric_limits<uint64_t>::max(), ossMax);

    EXPECT_EQ(oss0.str(), bytes({0,0,0,0,0,0,0,0}));
    EXPECT_EQ(ossMax.str(), bytes({0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}));
}

TEST(WriterTest, WriteU32Basic) {
    Writer w;
    std::ostringstream oss(std::ios::binary);

    uint32_t v = 0xA1B2C3D4u;
    w.writeU32(v, oss);

    std::string got = oss.str();
    ASSERT_EQ(got.size(), 4u);
    EXPECT_EQ(got, bytes({0xD4,0xC3,0xB2,0xA1}));
}

TEST(WriterTest, WriteStringWritesLenThenData) {
    Writer w;
    std::ostringstream oss(std::ios::binary);

    std::string s = "hello";
    w.writeString(s, oss);

    std::string got = oss.str();
    ASSERT_EQ(got.size(), 4 + s.size());

    std::string len = got.substr(0, 4);
    EXPECT_EQ(len, bytes({5,0,0,0}));

    std::string payload = got.substr(4);
    EXPECT_EQ(payload, s);
}

TEST(WriterTest, WriteStringEmpty) {
    Writer w;
    std::ostringstream oss(std::ios::binary);

    std::string s;
    w.writeString(s, oss);

    EXPECT_EQ(oss.str(), bytes({0,0,0,0}));
}

TEST(WriterTest, CopyBytesCopiesExactAmountWithVariousChunkSizes) {
    std::vector<uint8_t> data(1024);
    for (size_t i = 0; i < data.size(); ++i)
        data[i] = static_cast<uint8_t>(i & 0xFF);

    TempFile tf("writer", data);

    const std::vector<size_t> chunkSizes = {1, 3, 8, 64, 255, 256, 511, 512, 1024};
    const uint64_t N = 777;

    for (size_t chunk : chunkSizes) {
        std::ifstream in(tf.path, std::ios::binary);
        ASSERT_TRUE(in.is_open());

        std::ostringstream out(std::ios::binary);

        Writer w;
        w.copyBytes(in, N, out, chunk);

        auto got = out.str();
        ASSERT_EQ(got.size(), N);
        EXPECT_EQ(memcmp(got.data(), data.data(), static_cast<size_t>(N)), 0);
    }
}

TEST(WriterTest, CopyBytesThrowsOnShortInput) {
    std::vector<uint8_t> data = {1,2,3,4,5,6,7,8,9,10};
    TempFile tf("writer", data);

    std::ifstream in(tf.path, std::ios::binary);
    ASSERT_TRUE(in.is_open());

    std::ostringstream out(std::ios::binary);

    Writer w;
    EXPECT_THROW(w.copyBytes(in, 100, out, 4), std::runtime_error);
}
