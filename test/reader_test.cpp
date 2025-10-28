#include "reader.hpp"

#include "test_utils.hpp"

#include <gtest/gtest.h>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

using namespace std;


TEST(ReaderTest, ReadU32) {
    std::string data = bytes({0xD4, 0xC3, 0xB2, 0xA1});
    std::istringstream iss(data, std::ios::binary);

    Reader r;
    uint32_t v = r.readU32(iss);
    EXPECT_EQ(v, 0xA1B2C3D4u);
}

TEST(ReaderTest, ReadU64) {
    std::string data = bytes({0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01});
    std::istringstream iss(data, std::ios::binary);

    Reader r;
    uint64_t v = r.readU64(iss);
    EXPECT_EQ(v, 0x0102030405060708ULL);
}

TEST(ReaderTest, ReadU32ThrowsOnShortStream) {
    std::string data = bytes({0x11, 0x22, 0x33});
    std::istringstream iss(data, std::ios::binary);

    Reader r;
    EXPECT_THROW({ (void)r.readU32(iss); }, std::runtime_error);
}

TEST(ReaderTest, ReadU64ThrowsOnShortStream) {
    std::string data = bytes({0,1,2,3,4,5,6});
    std::istringstream iss(data, std::ios::binary);

    Reader r;
    EXPECT_THROW({ (void)r.readU64(iss); }, std::runtime_error);
}

TEST(ReaderTest, ReadStringEmpty) {
    std::string data = bytes({0,0,0,0});
    std::istringstream iss(data, std::ios::binary);

    Reader r;
    std::string s = r.readString(iss);
    EXPECT_TRUE(s.empty());
    EXPECT_TRUE(iss.peek() == std::char_traits<char>::eof());
}

TEST(ReaderTest, ReadString) {
    std::string payload = "hello";
    std::string data = bytes({5,0,0,0}) + payload;
    std::istringstream iss(data, std::ios::binary);

    Reader r;
    std::string s = r.readString(iss);
    EXPECT_EQ(s, payload);
    EXPECT_TRUE(iss.peek() == std::char_traits<char>::eof());
}

TEST(ReaderTest, ReadStringThrowsWhenNotEnoughDataForPayload) {
    std::string data = bytes({5,0,0,0}) + std::string("abc");
    std::istringstream iss(data, std::ios::binary);

    Reader r;
    EXPECT_THROW({ (void)r.readString(iss); }, std::runtime_error);
}

TEST(ReaderTest, SequentialReadsFromSingleStream) {
    std::string s = "OK";
    std::string data;
    data.reserve(4 + 8 + 4 + s.size());
    data += bytes({0xDD,0xCC,0xBB,0xAA});
    data += bytes({0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01});
    data += bytes({2,0,0,0});
    data += s;

    std::istringstream iss(data, std::ios::binary);

    Reader r;
    uint32_t a = r.readU32(iss);
    uint64_t b = r.readU64(iss);
    std::string c = r.readString(iss);

    EXPECT_EQ(a, 0xAABBCCDDu);
    EXPECT_EQ(b, 0x0102030405060708ULL);
    EXPECT_EQ(c, "OK");
    EXPECT_TRUE(iss.peek() == std::char_traits<char>::eof());
}
