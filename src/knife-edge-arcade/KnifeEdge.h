#pragma once
#include "Configuration.h"
#include "m64p/Core.h"

struct KnifeEdgeConfig
{ };

struct KnifeEdge
{
    KnifeEdge(const KnifeEdgeConfig& configuration, std::unique_ptr<m64p::Core>&& m64Core);
    void Run();
private:
    const std::unique_ptr<m64p::Core> m_m64Core;
};