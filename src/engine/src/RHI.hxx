#pragma once
#ifndef RHI_HXX
#    define RHI_HXX
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <platform.hxx>
#    include <engine.hxx>

#    define ENGINE_VERSION VK_MAKE_VERSION( 0, 0, 1 )

namespace Engine
{
    namespace tools
    {
        std::vector<VkPhysicalDevice> getAviliableDevices();
        std::vector<VkPhysicalDeviceProperties> getAviliableDevicesProperties();
    } // namespace tools
} // namespace Engine
#endif