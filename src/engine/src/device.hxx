#pragma once
#include <platform.hxx>
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <engine.hxx>

namespace Engine
{
    struct PhysicalDevice
    {
        PhysicalDevice();
        PhysicalDevice( const char *name, VkPhysicalDevice handle );
        const char *name{ nullptr };
        const VkPhysicalDevice getHandle() const;
        void setName( const char *name );
        void setHandle( VkPhysicalDevice device );

      private:
        VkPhysicalDevice _handle{ nullptr };
    };

    namespace tools
    {
        const std::vector<VkPhysicalDevice> getDevices();
        void createDevice( VkPhysicalDevice phDevice );
        void destroyDevice();
        const VkDevice getDevice();
        const VkPhysicalDevice getPhysicalDevice();
        void getSuitableDevices( std::vector<Device> &devices );
        void getSuitableDevices( std::vector<Device> &devices, uint8_t types );
        void getSuitableDevices( std::vector<PhysicalDevice> &devices );
        void getSuitableDevices( std::vector<PhysicalDevice> &devices, uint8_t types );
    } // namespace tools
} // namespace Engine
