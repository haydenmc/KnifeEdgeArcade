#pragma once
#include <filesystem>
#include <utility>
#include <toml++/toml.h>

struct Configuration
{
    Configuration();
    Configuration(std::filesystem::path configFile);
    std::filesystem::path GetConfigFilePath();
    bool GetFullscreen();
    std::pair<uint32_t, uint32_t> GetResolution();
    std::optional<std::filesystem::path> GetRomPath();
private:
    const std::filesystem::path m_configFilePath;
    const toml::table m_tomlTable;
};