#include "Mupen64PlusCore.h"

#include "Mupen64PlusUtil.h"
#include <m64p_types.h>

namespace
{
    constexpr uint32_t CORE_API_VERSION{ 0x020001 };
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
}

namespace m64p
{
    Mupen64PlusCore::Mupen64PlusCore(
        const Mupen64PlusCoreConfig& configuration
    ) : 
        m_configPath{ configuration.ConfigPath },
        m_dataPath{ configuration.DataPath }
    {
        CheckError(CoreStartup(
            CORE_API_VERSION,
            m_configPath.c_str(),
            m_dataPath.c_str(),
            this,
            Mupen64PlusCore::StaticDebugCallback,
            this,
            Mupen64PlusCore::StaticStateCallback
        ));
    }

    Mupen64PlusCore::~Mupen64PlusCore()
    {
        CoreShutdown();
    }

    void Mupen64PlusCore::StaticDebugCallback(
        void* context,
        int level,
        const char* message)
    {
        reinterpret_cast<m64p::Mupen64PlusCore*>(context)->DebugCallback(level, message);
    }

    void Mupen64PlusCore::DebugCallback(int level, const char* message)
    {
        // TODO
    }

    void Mupen64PlusCore::StaticStateCallback(
        void* context,
        m64p_core_param paramType,
        int newValue)
    {
        reinterpret_cast<m64p::Mupen64PlusCore*>(context)->StateCallback(paramType, newValue);
    }

    void Mupen64PlusCore::StateCallback(m64p_core_param paramType, int newValue)
    {
        // TODO
    }
}