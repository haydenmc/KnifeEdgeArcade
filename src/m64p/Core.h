// Wrapper for the mupen64plus-core C library
#pragma once
#include "Plugin.h"

#include <filesystem>
#include <m64p_types.h>
#include <string>

namespace m64p
{
    struct CoreConfig
    {
        std::string ConfigPath;
        std::string DataPath;
    };

    struct Core
    {
        Core(const CoreConfig& configuration, const std::filesystem::path& romFilePath);
        ~Core();

        void AttachVideoPlugin(std::unique_ptr<Plugin>&& videoPlugin);
        void AttachAudioPlugin(std::unique_ptr<Plugin>&& audioPlugin);
        void AttachInputPlugin(std::unique_ptr<Plugin>&& inputPlugin);
        void AttachRspPlugin(std::unique_ptr<Plugin>&& rspPlugin);
        void Execute();

    private:
        const std::string m_configPath;
        const std::string m_dataPath;
        const std::string m_romData;
        std::unique_ptr<Plugin> m_videoPlugin;
        std::unique_ptr<Plugin> m_audioPlugin;
        std::unique_ptr<Plugin> m_inputPlugin;
        std::unique_ptr<Plugin> m_rspPlugin;

        static void StaticDebugCallback(void* context, int level, const char* message);
        void DebugCallback(int level, const char* message);
        static void StaticStateCallback(void* context, m64p_core_param paramType, int newValue);
        void StateCallback(m64p_core_param paramType, int newValue);
    };
}