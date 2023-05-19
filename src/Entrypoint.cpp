#include <pch.h>
#include <cstdlib>
#include <iostream>
#include "m64p/Core.h"
#include "m64p/Plugin.h"

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
    std::filesystem::path romPath{
        "C:\\Users\\Hayden\\OneDrive\\Software\\Games\\N64\\Knife Edge - Nose Gunner (USA).n64" };
    spdlog::info("Starting mupen64plus-core with configuration and data path: '{}'...", configPath);
    m64p::Core m64{
        {
            configPath, // Config path
            configPath, // Data path
        },
        romPath};

    std::string videoPluginPath{ "mupen64plus-video-glide64mk2.dll" };
    auto videoPlugin{ std::make_unique<m64p::Plugin>(videoPluginPath) };
    m64.AttachVideoPlugin(std::move(videoPlugin));

    std::string audioPluginPath{ "mupen64plus-audio-sdl.dll" };
    auto audioPlugin{ std::make_unique<m64p::Plugin>(audioPluginPath) };
    m64.AttachAudioPlugin(std::move(audioPlugin));

    std::string inputPluginPath{ "mupen64plus-input-sdl.dll" };
    auto inputPlugin{ std::make_unique<m64p::Plugin>(inputPluginPath) };
    m64.AttachInputPlugin(std::move(inputPlugin));

    std::string rspPluginPath{ "mupen64plus-rsp-hle.dll" };
    auto rspPlugin{ std::make_unique<m64p::Plugin>(rspPluginPath) };
    m64.AttachRspPlugin(std::move(rspPlugin));

    m64.Execute();

    return 0;
}