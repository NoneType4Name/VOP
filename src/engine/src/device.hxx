#pragma once
#include <platform.hxx>
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <descriptorPool.hxx>
#include <surface.hxx>
#include <queue.hxx>

namespace Engine
{
    struct DeviceDescription::DATA_TYPE
    {
        DATA_TYPE() = default;
        void init( VkPhysicalDevice device );
        std::vector<VkQueueFamilyProperties> queueFamilyProperties;
        VkPhysicalDeviceMemoryProperties memProperties {};
        VkPhysicalDeviceProperties properties {};
        VkPhysicalDeviceFeatures features {};
        VkPhysicalDevice phDevice { nullptr };
    };

    struct device::DATA_TYPE
    {
        DATA_TYPE( DeviceDescription *description );
        virtual void setupNextChain( const void *&pNext, std::vector<void *> &dataPointers, void *userPoiner );
        virtual void setupExtensions( std::vector<const char *> &deviceExtensions, void *userPoiner );
        virtual void setupFeatures( VkPhysicalDeviceFeatures &features, void *userPoiner );
        virtual void setupQueueSet( queueSet &queues, VkSurfaceKHR surface, void *userPoiner );
        virtual void setupDescriptors( VkDevice device, std::vector<descriptorPool> &descriptorSets );
        void setExtensions( std::vector<const char *> &deviceExtensions );
        bool supportExtensions();
        void init();
        ~DATA_TYPE();
        VkDevice device { nullptr };
        VkCommandPool grapchicPool { nullptr };
        VkCommandPool transferPool { nullptr };
        VkCommandPool presentPool { nullptr };
        std::vector<descriptorPool> descriptorPools;
        std::vector<const char *> extensions;
        window::window *window { nullptr };
        queueSet queuesSet;
        DeviceDescription *description { nullptr };
    };

    namespace tools
    {
        inline DeviceType VkDevTypeToEngineDevType( VkPhysicalDeviceType type );
        uint32_t requeredMemoryTypeIndex( types::device device, uint32_t type, VkMemoryPropertyFlags properties );
    } // namespace tools
} // namespace Engine