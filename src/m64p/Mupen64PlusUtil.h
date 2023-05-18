#pragma once
#include <format>
#include <m64p_types.h>
#include <stdexcept>

namespace m64p
{
    static inline void CheckError(const m64p_error& error)
    {
        if (error != M64ERR_SUCCESS)
        {
            throw std::runtime_error(std::format(
                "Mupen64plus error {}",
                static_cast<uint32_t>(error)));
        }
    }
}