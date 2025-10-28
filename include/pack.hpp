#pragma once

#include <filesystem>

struct Config;

int packCmd(const Config& config, const std::filesystem::path& inputDir, const std::filesystem::path& archivePath);
