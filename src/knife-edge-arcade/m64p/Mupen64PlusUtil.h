#pragma once
#include <fmt/format.h>
#include <m64p_types.h>
#include <stdexcept>

namespace m64p
{
    static inline void ThrowIfError(const m64p_error& error)
    {
        if (error != M64ERR_SUCCESS)
        {
            throw std::runtime_error(fmt::format(
                "Mupen64plus error {}",
                static_cast<uint32_t>(error)));
        }
    }
}