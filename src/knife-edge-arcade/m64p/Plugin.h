#pragma once
#include <string>

namespace m64p
{
    // Platform-specific module/library types
#ifdef _WIN32
        #include <minwindef.h>
        using PluginSharedLibrary = HMODULE;
#else
        using PluginSharedLibrary = void*;
#endif

    // Plugin functions
    using PluginStartupFunction = m64p_error (*)(
        m64p_dynlib_handle CoreLibHandle,
        void *Context,
        void (*DebugCallback)(void *context, int level, const char *message));
    using PluginShutdownFunction = m64p_error (*)();
    using PluginGetVersionFunction = m64p_error (*)(
        m64p_plugin_type* pluginType,
        int* pluginVersion,
        int *apiVersion,
        const char** pluginName,
        int *capabilities);

    struct PluginVersion
    {
        m64p_plugin_type Type;
        uint8_t Major;
        uint8_t Minor;
        uint8_t Revision;
        uint32_t ApiVersion;
        std::string Name;
        uint32_t Capabilities;
    };

    struct Plugin
    {
        Plugin(std::string libraryPath);
        virtual ~Plugin();

        m64p_dynlib_handle GetPluginLibrary();
        PluginVersion GetPluginVersion();

    private:
        const PluginSharedLibrary m_pluginLibrary{ nullptr };
        const PluginStartupFunction m_startupFunction;
        const PluginShutdownFunction m_shutdownFunction;
        const PluginGetVersionFunction m_getVersionFunction;

        void Startup();
        static void StaticDebugCallback(void* context, int level, const char* message);
        void DebugCallback(int level, const char* message);
        void Shutdown();
    };
}