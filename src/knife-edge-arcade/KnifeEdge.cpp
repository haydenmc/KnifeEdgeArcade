#include <pch.h>
#include "KnifeEdge.h"

KnifeEdge::KnifeEdge(const KnifeEdgeConfig &configuration, std::unique_ptr<m64p::Core> &&m64Core) :
    m_m64Core{ std::move(m64Core) }
{
}

void KnifeEdge::Run()
{
    m_m64Core->Execute();
}
