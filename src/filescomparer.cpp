#include "filescomparer.hpp"

#include <fstream>
#include <vector>
#include <cstring>


bool DefaultFileComparer::areEqual(const std::filesystem::path &path1, const std::filesystem::path &path2, size_t buf_bytes)
{
    std::ifstream i1(path1, std::ios::binary), i2(path2, std::ios::binary);
    if(!i1 || !i2) return false;
    std::vector<char> A(buf_bytes), B(buf_bytes);
    i1.rdbuf()->pubsetbuf(A.data(), A.size());
    i2.rdbuf()->pubsetbuf(B.data(), B.size());
    for(;;){
        i1.read(A.data(), A.size());
        i2.read(B.data(), B.size());
        auto ga = i1.gcount(), gb = i2.gcount();
        if (ga != gb)
        {
            return false;
        }
        if (ga == 0)
        {
            return true;
        }
        if (std::memcmp(A.data(), B.data(), (size_t)ga) != 0)
        {
            return false;
        }
    }
}
