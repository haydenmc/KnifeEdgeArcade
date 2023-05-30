#include <pch.h>
#include <fstream>
#include "Configuration.h"

namespace
{
    constexpr std::string_view CONFIG_FILE_NAME{ "config.toml" };
    constexpr bool DEFAULT_FULLSCREEN{ false };
    constexpr std::pair<uint32_t, uint32_t> DEFAULT_RESOLUTION{ 1280, 720 };

    std::filesystem::path GetDefaultConfigFile()
    {
        std::filesystem::path appDataDirectoryPath;
        if (auto appDataVar{ std::getenv("APPDATA") })
        {
            appDataDirectoryPath = std::filesystem::path{ appDataVar } / "knife-edge-arcade";
        }
        else if (auto homeVar{ std::getenv("HOME") })
        {
            appDataDirectoryPath = std::filesystem::path{ homeVar } / "knife-edge-arcade";
        }
        else
        {
            spdlog::warn("Could not find APPDATA or HOME environment variables, "
                "using current directory for configuration files.");
            appDataDirectoryPath = std::filesystem::current_path();
        }
        if (!std::filesystem::exists(appDataDirectoryPath))
        {
            if (!std::filesystem::create_directory(appDataDirectoryPath))
            {
                throw std::runtime_error(fmt::format(
                    "Could not create configuration directory '{}'",
                    appDataDirectoryPath.string()));
            }
        }
        return appDataDirectoryPath / CONFIG_FILE_NAME;
    }

    toml::table ReadConfigFile(std::filesystem::path configFilePath)
    {
        if (!std::filesystem::exists(configFilePath))
        {
            std::ofstream outfile{ configFilePath.string() };
            outfile.close();
            spdlog::info("Created blank configuration file at '{}'", configFilePath.string());
        }
        spdlog::info("Reading configuration from '{}'...", configFilePath.string());
        return toml::parse_file(configFilePath.string());
    }
}

Configuration::Configuration() : Configuration(GetDefaultConfigFile())
{ }

Configuration::Configuration(std::filesystem::path configFile) : m_configFilePath{ configFile },
    m_tomlTable{ ReadConfigFile(m_configFilePath) }
{ }

std::filesystem::path Configuration::GetConfigFilePath()
{
    return m_configFilePath;
}

bool Configuration::GetFullscreen()
{
    if (auto configNode{ m_tomlTable.at_path("fullscreen") })
    {
        if (auto configValue{ configNode.as_boolean() })
        {
            return configValue->get();
        }
    }
    
    return DEFAULT_FULLSCREEN;
}

std::pair<uint32_t, uint32_t> Configuration::GetResolution()
{
    if (auto resolutionNode{ m_tomlTable.get("resolution") })
    {
        if (auto resolutionArray{ resolutionNode->as_array() })
        {
            if (resolutionArray->is_homogeneous(toml::v3::node_type::integer) &&
                (resolutionArray->size() == 2))
            {
                return { static_cast<uint32_t>(resolutionArray->get_as<int64_t>(0)->get()),
                    static_cast<uint32_t>(resolutionArray->get_as<int64_t>(1)->get()) };
            }
        }
    }
    return DEFAULT_RESOLUTION;
}

std::optional<std::filesystem::path> Configuration::GetRomPath()
{
    if (auto romPath{ m_tomlTable.get_as<std::string>("rom_path") })
    {
        return std::filesystem::path{ romPath->get() };
    }
    return std::nullopt;
}
