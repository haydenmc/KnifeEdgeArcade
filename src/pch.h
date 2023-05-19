#pragma once
#include <string>
#include <filesystem>
#include <fmt/format.h>
#include <m64p_types.h>
#include <spdlog/spdlog.h>
#ifdef _WIN32
    #define VC_EXTRALEAN
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif