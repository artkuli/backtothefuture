#pragma once

#include <filesystem>

namespace packager
{

int pack(const std::filesystem::path& input_dir, const std::filesystem::path& archive_path);

}
