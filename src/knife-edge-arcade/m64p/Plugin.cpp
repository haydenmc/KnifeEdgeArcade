#include <pch.h>
#include "Mupen64PlusUtil.h"
#include "Plugin.h"

namespace
{
    // Platform-specific code for loading shared libraries
#ifdef _WIN32
    m64p_dynlib_handle GetCoreLibraryHandle()
    {
        HMODULE result{ GetModuleHandleA("mupen64plus.dll") };
        if (result == nullptr)
        {
            throw std::runtime_error(fmt::format(
                "Could not find mupen64plus core library: error {}", GetLastError()));
        }
        return result;
    }

    m64p::PluginSharedLibrary LoadPluginLibrary(const std::string& libraryPath)
    {
        HMODULE result{ LoadLibraryA(libraryPath.c_str()) };
        if (result == nullptr)
        {
            throw std::runtime_error(fmt::format(
                "Could not load library '{}': error {}",
                libraryPath, GetLastError()));
        }
        return result;
    }

    void* FindPluginFunction(const HMODULE& hModule, const std::string& functionName)
    {
        FARPROC result{ GetProcAddress(hModule, functionName.c_str()) };
        if (result == nullptr)
        {
            throw std::runtime_error(fmt::format(
                "Could not locate function '{}' while loading shared library: error {}",
                functionName, GetLastError()));
        }
        return result;
    }
#else
    m64p_dynlib_handle GetCoreLibraryHandle()
    {
        // TODO
    }

    m64p::PluginSharedLibrary LoadPluginLibrary(const std::string& libraryPath)
    {
        // TODO
    }

    void* FindPluginFunction(const std::string& functionName)
    {
        // TODO
    }
#endif

}

namespace m64p
{
    Plugin::Plugin(std::string libraryPath) :
        m_pluginLibrary{ LoadPluginLibrary(libraryPath) },
        m_startupFunction{ reinterpret_cast<PluginStartupFunction>(
            FindPluginFunction(m_pluginLibrary, "PluginStartup")) },
        m_shutdownFunction{ reinterpret_cast<PluginShutdownFunction>(
            FindPluginFunction(m_pluginLibrary, "PluginShutdown")) },
        m_getVersionFunction{ reinterpret_cast<PluginGetVersionFunction>(
            FindPluginFunction(m_pluginLibrary, "PluginGetVersion")) }
    {
        spdlog::info("Loading Mupen64Plus plugin {}...", libraryPath);
        Startup();
    }

    Plugin::~Plugin()
    {
        spdlog::info("Plugin shutting down...");
        Shutdown();
    }

    m64p_dynlib_handle Plugin::GetPluginLibrary()
    {
        return m_pluginLibrary;
    }

    PluginVersion Plugin::GetPluginVersion()
    {
        m64p_plugin_type type;
        int32_t pluginVersion;
        int32_t apiVersion;
        const char* pluginName;
        int32_t capabilities;
        CheckError(m_getVersionFunction(
            &type, &pluginVersion, &apiVersion, &pluginName, &capabilities));
        return PluginVersion
        {
            type,
            static_cast<uint8_t>((pluginVersion >> 16) & 0x000000FF),
            static_cast<uint8_t>((pluginVersion >> 8) & 0x000000FF),
            static_cast<uint8_t>(pluginVersion & 0x000000FF),
            static_cast<uint32_t>(apiVersion),
            std::string{ pluginName },
            static_cast<uint32_t>(capabilities),
        };
    }

    void Plugin::Startup()
    {
        m_startupFunction(GetCoreLibraryHandle(), this, &Plugin::StaticDebugCallback);
    }

    void Plugin::StaticDebugCallback(void* context, int level, const char* message)
    {
        reinterpret_cast<Plugin*>(context)->DebugCallback(level, message);
    }

    void Plugin::DebugCallback(int level, const char* message)
    {
        if (level == M64MSG_ERROR)
        {
            spdlog::error("mupen64plus-plugin: {}", message);
        }
        else if (level == M64MSG_WARNING)
        {
            spdlog::warn("mupen64plus-plugin: {}", message);
        }
        else if (level == M64MSG_INFO || level == M64MSG_STATUS)
        {
            spdlog::info("mupen64plus-plugin: {}", message);
        }
        else
        {
            spdlog::debug("mupen64plus-plugin: {}", message);
        }
    }

    void Plugin::Shutdown()
    {
        CheckError(m_shutdownFunction());
    }
}