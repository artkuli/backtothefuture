#include "utils.hpp"

#include <array>

namespace utils {

using perms = std::filesystem::perms;

namespace {

enum class BitPos : int {
    URead = 0, UWrite = 1, UExec = 2,
    GRead = 3, GWrite = 4, GExec = 5,
    ORead = 6, OWrite = 7, OExec = 8,
};

inline constexpr std::array<std::pair<std::filesystem::perms, BitPos>, 9> kPermsArray{{
  {std::filesystem::perms::owner_read,   BitPos::URead},
  {std::filesystem::perms::owner_write,  BitPos::UWrite},
  {std::filesystem::perms::owner_exec,   BitPos::UExec},
  {std::filesystem::perms::group_read,   BitPos::GRead},
  {std::filesystem::perms::group_write,  BitPos::GWrite},
  {std::filesystem::perms::group_exec,   BitPos::GExec},
  {std::filesystem::perms::others_read,  BitPos::ORead},
  {std::filesystem::perms::others_write, BitPos::OWrite},
  {std::filesystem::perms::others_exec,  BitPos::OExec},
}};

} // anonymous namespace

int64_t filetime_to_time_t(std::filesystem::file_time_type tp)
{
    using namespace std::chrono;
    auto sctp = time_point_cast<system_clock::duration>(
        tp
        - std::filesystem::file_time_type::clock::now()
        + system_clock::now());
    return (int64_t)system_clock::to_time_t(sctp);
}

std::filesystem::file_time_type time_t_to_filetime(int64_t t)
{
    using namespace std::chrono;
    auto sctp = system_clock::from_time_t((time_t)t);
    return std::filesystem::file_time_type::clock::now() + (sctp - system_clock::now());
}

uint32_t perms_to_bits(std::filesystem::perms permission)
{
    uint32_t retValue = 0;
    for (const auto&[flag, position] : kPermsArray) {
        if ((permission & flag) != perms::none) {
            retValue |= (1u << static_cast<int>(position));
        }
    }
    return retValue;
}

std::filesystem::perms bits_to_perms(uint32_t bits)
{
    perms retValue = perms::none;
    bits &= (1u << 9) - 1u;
    for (const auto& [flag, position] : kPermsArray) {
        if (bits & (1u << static_cast<int>(position))) {
            retValue |= flag;
        }
    }
   return retValue;
}

} // utils
