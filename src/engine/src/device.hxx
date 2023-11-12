#pragma once
#include <platform.hxx>
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <engine.hxx>
#include <surface.hxx>
#include <queue.hxx>

namespace Engine
{
    // struct PhysicalDevice
    // {
    //     PhysicalDevice();
    //     PhysicalDevice( const char *name, VkPhysicalDevice handle );
    //     const char *name { nullptr };
    //     const VkPhysicalDevice getHandle() const;
    //     void setName( const char *name );
    //     void setHandle( VkPhysicalDevice device );

    //   private:
    //     VkPhysicalDevice _handle { nullptr };
    // };

    namespace tools
    {
        inline DeviceType VkDevTypeToEngineDevType( VkPhysicalDeviceType type );
        uint32_t requeredMemoryTypeIndex( types::device device, uint32_t type, VkMemoryPropertyFlags properties );
    } // namespace tools
} // namespace Engine