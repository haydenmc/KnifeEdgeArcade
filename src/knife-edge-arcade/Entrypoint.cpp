#include <pch.h>
#include <cstdlib>
#include <iostream>
#include "Configuration.h"
#include "KnifeEdge.h"
#include "m64p/Core.h"
#include "m64p/Plugin.h"

namespace
{
    inline std::unique_ptr<m64p::Core> InitializeMupen64Plus(
        const std::shared_ptr<Configuration>& config)
    {
        auto romFilePath{ config->GetRomPath() };
        if (!romFilePath)
        {
            spdlog::error("No rom file path defined in configuration! "
                "Please define 'rom_path' in file '{}'.",
                config->GetConfigFilePath().string());
            std::exit(EXIT_FAILURE);
        }
        m64p::CoreConfig coreConfig{
            .DisplayFullscreen = config->GetFullscreen(),
            .DisplayResolution = config->GetResolution(),
            .RomFilePath = romFilePath.value()
        };
        auto m64{ std::make_unique<m64p::Core>(coreConfig) };

        std::string videoPluginPath{ "mupen64plus-video-glide64mk2.dll" };
        auto videoPlugin{ std::make_unique<m64p::Plugin>(videoPluginPath) };
        m64->AttachVideoPlugin(std::move(videoPlugin));

        std::string audioPluginPath{ "mupen64plus-audio-sdl.dll" };
        auto audioPlugin{ std::make_unique<m64p::Plugin>(audioPluginPath) };
        m64->AttachAudioPlugin(std::move(audioPlugin));

        std::string inputPluginPath{ "mupen64plus-input-sdl.dll" };
        auto inputPlugin{ std::make_unique<m64p::Plugin>(inputPluginPath) };
        m64->AttachInputPlugin(std::move(inputPlugin));

        std::string rspPluginPath{ "mupen64plus-rsp-hle.dll" };
        auto rspPlugin{ std::make_unique<m64p::Plugin>(rspPluginPath) };
        m64->AttachRspPlugin(std::move(rspPlugin));

        return m64;
    }
}

int main()
{
#ifdef _DEBUG
    spdlog::set_level(spdlog::level::level_enum::debug);
#endif
    spdlog::info("Starting up...");
    auto configuration{ std::make_shared<Configuration>() };
    spdlog::info("Initializing Mupen64Plus...");
    auto m64Core{ InitializeMupen64Plus(configuration) };
    spdlog::info("Initializing KnifeEdge...");
    auto knifeEdge{ std::make_unique<KnifeEdge>(KnifeEdgeConfig{}, std::move(m64Core)) };
    spdlog::info("Starting emulation...");
    knifeEdge->Run();
    spdlog::info("Emulation finished.");
    return 0;
}