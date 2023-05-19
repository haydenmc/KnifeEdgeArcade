#include <pch.h>
#include <cstdlib>
#include <iostream>
#include "m64p/Mupen64PlusCore.h"

int main()
{
#ifdef _DEBUG
    spdlog::set_level(spdlog::level::level_enum::debug);
#endif
    spdlog::info("Starting up...");
    std::string configPath;
    if (auto appDataVar{ std::getenv("APPDATA") })
    {
        configPath = fmt::format("{}/KnifeEdgeArcade", appDataVar);
    }
    else if (auto homeVar{ std::getenv("HOME") })
    {
        configPath = fmt::format("{}/.knife-edge-arcade", homeVar);
    }
    spdlog::info("Starting mupen64plus-core with configuration and data path: '{}'...", configPath);
    m64p::Mupen64PlusCore m64{{
        configPath, // Config path
        configPath, // Data path
    }};
}