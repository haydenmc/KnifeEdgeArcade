#include <pch.h>
#include <atomic>
#include <fstream>
#include <m64p_types.h>
#include "Core.h"
#include "Mupen64PlusUtil.h"

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
    extern m64p_error ConfigSetParameter(m64p_handle ConfigSectionHandle,
        const char *ParamName, m64p_type ParamType, const void *ParamValue);
    extern m64p_error ConfigOpenSection(const char *SectionName,
        m64p_handle *ConfigSectionHandle);
}

namespace
{
    std::atomic<m64p::Core*> g_coreInstance{ nullptr };
    constexpr uint32_t CORE_API_VERSION{ 0x020001 };

    inline std::vector<std::byte> ReadFile(const std::filesystem::path& filePath)
    {
        std::basic_ifstream<std::byte> file{ filePath, std::ios::in | std::ios::binary };
        return std::vector<std::byte>{ (std::istreambuf_iterator<std::byte>(file)), {} };
    }

    inline void ApplyConfiguration(const m64p::CoreConfig& config)
    {
        m64p_handle videoConfigSection;
        m64p::ThrowIfError(ConfigOpenSection("Video-General", &videoConfigSection));
        m64p::ThrowIfError(ConfigSetParameter(videoConfigSection, "ScreenWidth", M64TYPE_INT,
            &(config.DisplayResolution.first)));
        m64p::ThrowIfError(ConfigSetParameter(videoConfigSection, "ScreenHeight", M64TYPE_INT,
            &(config.DisplayResolution.second)));
        uint32_t fullscreen{ config.DisplayFullscreen ? 1u : 0u };
        m64p::ThrowIfError(ConfigSetParameter(videoConfigSection, "Fullscreen", M64TYPE_BOOL,
            &fullscreen));
    }
}

namespace m64p
{
    Core::Core(const CoreConfig& configuration) : m_romData{ ReadFile(configuration.RomFilePath) }
    {
        if (g_coreInstance.exchange(this) != nullptr)
        {
            throw std::runtime_error("Only 1 instance of Core can be running.");
        }
        ThrowIfError(CoreStartup(CORE_API_VERSION, nullptr, nullptr,
            this, Core::StaticDebugCallback, this, Core::StaticStateCallback));
        ApplyConfiguration(configuration);
        ThrowIfError(CoreDoCommand(m64p_command::M64CMD_ROM_OPEN,
            m_romData.size(), const_cast<std::byte*>(m_romData.data())));
    }

    Core::~Core()
    {
        auto shutdownResult{ CoreShutdown() };
        if (shutdownResult != M64ERR_SUCCESS)
        {
            spdlog::error("Core: Error {} returned from CoreShutdown.");
        }
        if (g_coreInstance.exchange(nullptr) != this)
        {
            spdlog::error("Core: Unexpected value held in core singleton pointer.");
        }
    }

    void Core::AttachVideoPlugin(std::unique_ptr<Plugin>&& videoPlugin)
    {
        spdlog::info("Core: Attaching video plugin...");
        m_videoPlugin = std::move(videoPlugin);
        ThrowIfError(CoreAttachPlugin(m64p_plugin_type::M64PLUGIN_GFX,
            m_videoPlugin->GetPluginLibrary()));
    }

    void Core::AttachAudioPlugin(std::unique_ptr<Plugin>&& audioPlugin)
    {
        spdlog::info("Core: Attaching audio plugin...");
        m_audioPlugin = std::move(audioPlugin);
        ThrowIfError(CoreAttachPlugin(m64p_plugin_type::M64PLUGIN_AUDIO,
            m_audioPlugin->GetPluginLibrary()));
    }

    void Core::AttachInputPlugin(std::unique_ptr<Plugin>&& inputPlugin)
    {
        spdlog::info("Core: Attaching input plugin...");
        m_inputPlugin = std::move(inputPlugin);
        ThrowIfError(CoreAttachPlugin(m64p_plugin_type::M64PLUGIN_INPUT,
            m_inputPlugin->GetPluginLibrary()));
    }

    void Core::AttachRspPlugin(std::unique_ptr<Plugin>&& rspPlugin)
    {
        spdlog::info("Core: Attaching RSP plugin...");
        m_rspPlugin = std::move(rspPlugin);
        ThrowIfError(CoreAttachPlugin(m64p_plugin_type::M64PLUGIN_RSP,
            m_rspPlugin->GetPluginLibrary()));
    }

    void Core::SetOnFrameCallback(std::function<void()> onFrameCallback)
    {
        m_onFrameCallback = onFrameCallback;
        if (m_onFrameCallback)
        {
            CoreDoCommand(m64p_command::M64CMD_SET_FRAME_CALLBACK, 0, &Core::StaticFrameCallback);
            spdlog::info("Core: OnFrameCallback set");
        }
        else
        {
            CoreDoCommand(m64p_command::M64CMD_SET_FRAME_CALLBACK, 0, nullptr);
            spdlog::info("Core: OnFrameCallback cleared");
        }
    }

    void Core::Execute()
    {
        spdlog::info("Core: Executing emulation...");
        ThrowIfError(CoreDoCommand(m64p_command::M64CMD_EXECUTE, 0, nullptr));
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

    void Core::StaticFrameCallback()
    {
        if (!g_coreInstance)
        {
            spdlog::error("Core: Frame callback invoked without a valid Core instance.");
            return;
        }
        g_coreInstance.load()->FrameCallback();
    }

    void Core::FrameCallback()
    {
        if (!m_onFrameCallback)
        {
            spdlog::error("Core: FrameCallback was called, but no callback was registered.");
            return;
        }
        m_onFrameCallback();
    }
}