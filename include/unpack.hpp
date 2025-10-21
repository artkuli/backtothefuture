#pragma once

#include <filesystem>

namespace Unpackager
{

int unpack(const std::filesystem::path& archive_path, const std::filesystem::path& output_dir);

}
