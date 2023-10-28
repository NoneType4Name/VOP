#include <EHI.hxx>

namespace Engine
{
    namespace
    {
        std::vector<DeviceDescription> _devicesDesc;
    }

    std::vector<DeviceDescription> &tools::setupDeviceDescriptions()
    {
        if ( !_devicesDesc.size() )
            goto ret;

    ret:
        return _devicesDesc;
    }
} // namespace Engine