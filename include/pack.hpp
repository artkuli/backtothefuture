#pragma once

#include <filesystem>

namespace Packager
{

int pack(const std::filesystem::path& input_dir, const std::filesystem::path& archive_path);

}
