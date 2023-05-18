// Wrapper for the mupen64plus-core C library
#pragma once
#include <m64p_types.h>
#include <string>

namespace m64p
{
    struct Mupen64PlusCoreConfig
    {
        std::string ConfigPath;
        std::string DataPath;
    };

    struct Mupen64PlusCore
    {
        Mupen64PlusCore(const Mupen64PlusCoreConfig& configuration);
        ~Mupen64PlusCore();
        
    private:
        const std::string m_configPath;
        const std::string m_dataPath;

        static void StaticDebugCallback(void* context, int level, const char* message);
        void DebugCallback(int level, const char* message);
        static void StaticStateCallback(void* context, m64p_core_param paramType, int newValue);
        void StateCallback(m64p_core_param paramType, int newValue);
    };
}