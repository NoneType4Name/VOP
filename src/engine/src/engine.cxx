#include <engine.hxx>
#include <RHI.hxx>

namespace Engine
{
    void setup()
    {
    }

    void init( AppCreateInfo sAppCreateInfo )
    {
        tools::createInstance();
    }

    void shutdown()
    {
        tools::destroyInstance();
    }
} // namespace Engine