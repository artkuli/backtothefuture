#pragma once

#include <filesystem>

struct Config;

int unpackCmd(const Config& config,
           const std::filesystem::path& archive_path,
           const std::filesystem::path& output_dir);
