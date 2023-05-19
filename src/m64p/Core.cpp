#include <pch.h>
#include <fstream>
#include <m64p_types.h>
#include "Core.h"
#include "Mupen64PlusUtil.h"

namespace
{
    constexpr uint32_t CORE_API_VERSION{ 0x020001 };

    inline std::string ReadFile(const std::filesystem::path& filePath)
    {
        std::ifstream file{ filePath, std::ifstream::in | std::ifstream::binary };
        return std::string{ (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>() };
    }
}

extern "C"
{
    // Mupen64Plus Core front-end api
    extern void DebugMessage(
        int level,
        const char *message,
        ...);
    extern void DebugCallback(
        void* context,
        int level,
        const char* message);
    extern void StateCallback(
        void* context,
        m64p_core_param paramType,
        int newValue);
    // m64p_frontend.h
    extern m64p_error CoreStartup(
        int apiVersion,
        const char* configPath,
        const char* dataPath,
        void* context,
        void (*DebugCallback)(
            void* context,
            int level,
            const char* message),
        void* context2, 
        void (*StateCallback)(
            void* context,
            m64p_core_param paramType,
            int newValue
        ));
    extern m64p_error CoreShutdown();
    extern m64p_error CoreAttachPlugin(
        m64p_plugin_type PluginType,
        m64p_dynlib_handle PluginLibHandle);
    extern m64p_error CoreDetachPlugin(m64p_plugin_type pluginType);
    extern m64p_error CoreDoCommand(m64p_command command, int paramInt, void* paramPtr);
}

namespace m64p
{
    Core::Core(
        const CoreConfig& configuration,
        const std::filesystem::path& romFilePath
    ) : 
        m_configPath{ configuration.ConfigPath },
        m_dataPath{ configuration.DataPath },
        m_romData{ ReadFile(romFilePath) }
    {
        CheckError(CoreStartup(
            CORE_API_VERSION,
            m_configPath.c_str(),
            m_dataPath.c_str(),
            this,
            Core::StaticDebugCallback,
            this,
            Core::StaticStateCallback
        ));

        CheckError(CoreDoCommand(m64p_command::M64CMD_ROM_OPEN,
            m_romData.size(), const_cast<char*>(m_romData.data())));
    }

    Core::~Core()
    {
        CheckError(CoreShutdown());
    }

    void Core::AttachVideoPlugin(std::unique_ptr<Plugin>&& videoPlugin)
    {
        spdlog::info("Core: Attaching video plugin...");
        m_videoPlugin = std::move(videoPlugin);
        CheckError(CoreAttachPlugin(m64p_plugin_type::M64PLUGIN_GFX,
            m_videoPlugin->GetPluginLibrary()));
    }

    void Core::AttachAudioPlugin(std::unique_ptr<Plugin>&& audioPlugin)
    {
        spdlog::info("Core: Attaching audio plugin...");
        m_audioPlugin = std::move(audioPlugin);
        CheckError(CoreAttachPlugin(m64p_plugin_type::M64PLUGIN_AUDIO,
            m_audioPlugin->GetPluginLibrary()));
    }

    void Core::AttachInputPlugin(std::unique_ptr<Plugin>&& inputPlugin)
    {
        spdlog::info("Core: Attaching input plugin...");
        m_inputPlugin = std::move(inputPlugin);
        CheckError(CoreAttachPlugin(m64p_plugin_type::M64PLUGIN_INPUT,
            m_inputPlugin->GetPluginLibrary()));
    }

    void Core::AttachRspPlugin(std::unique_ptr<Plugin>&& rspPlugin)
    {
        spdlog::info("Core: Attaching RSP plugin...");
        m_rspPlugin = std::move(rspPlugin);
        CheckError(CoreAttachPlugin(m64p_plugin_type::M64PLUGIN_RSP,
            m_rspPlugin->GetPluginLibrary()));
    }

    void Core::Execute()
    {
        spdlog::info("Core: Executing emulation...");
        CheckError(CoreDoCommand(m64p_command::M64CMD_EXECUTE, 0, nullptr));
    }

    void Core::StaticDebugCallback(
        void* context,
        int level,
        const char* message)
    {
        reinterpret_cast<m64p::Core*>(context)->DebugCallback(level, message);
    }

    void Core::DebugCallback(int level, const char* message)
    {
        if (level == M64MSG_ERROR)
        {
            spdlog::error("mupen64plus-core: {}", message);
        }
        else if (level == M64MSG_WARNING)
        {
            spdlog::warn("mupen64plus-core: {}", message);
        }
        else if (level == M64MSG_INFO || level == M64MSG_STATUS)
        {
            spdlog::info("mupen64plus-core: {}", message);
        }
        else
        {
            spdlog::debug("mupen64plus-core: {}", message);
        }
    }

    void Core::StaticStateCallback(
        void* context,
        m64p_core_param paramType,
        int newValue)
    {
        reinterpret_cast<m64p::Core*>(context)->StateCallback(paramType, newValue);
    }

    void Core::StateCallback(m64p_core_param paramType, int newValue)
    {
        // TODO
    }
}