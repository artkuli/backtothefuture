#pragma once

#include <filesystem>

namespace utils {

int64_t filetime_to_time_t(std::filesystem::file_time_type tp);

std::filesystem::file_time_type time_t_to_filetime(int64_t t);

uint32_t perms_to_bits(std::filesystem::perms p);

std::filesystem::perms bits_to_perms(uint32_t b);

} // utils
