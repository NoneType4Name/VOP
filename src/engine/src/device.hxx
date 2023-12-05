#pragma once
#ifndef DEVICE_HXX
#    define DEVICE_HXX
#    include <platform.hxx>
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <descriptorPool.hxx>
#    include <surface.hxx>
#    include <queue.hxx>

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
        void setExtensions( std::vector<const char *> &deviceExtensions );
        bool supportExtensions();
        uint32_t setImageMemory( VkImage &image, VkMemoryPropertyFlags properties );
        void resetImageMemory( VkImage &image, uint32_t index );
        void addImagesMemorySize( uint32_t index, uint32_t size );
        uint32_t setBufferMemory( VkBuffer &buffer, VkMemoryPropertyFlags properties );
        void resetBufferMemory( VkBuffer &buffer, uint32_t index );
        void addBuffersMemorySize( uint32_t index, uint32_t size );
        ~DATA_TYPE();
        VkDevice device { nullptr };
        VkCommandPool grapchicPool { nullptr };
        VkCommandPool transferPool { nullptr };
        VkCommandPool presentPool { nullptr };
        std::vector<const char *> extensions;
        std::vector<std::unique_ptr<descriptorPool>> descriptorPools;
        std::vector<std::unique_ptr<shader>> shaders;
        std::vector<std::unique_ptr<layout>> layouts;
        std::vector<std::unique_ptr<pipeline>> pipelines;
        // memoryTypeIndex<<handle, offset>, <size, oversize>>;
        std::vector<std::pair<std::unordered_map<VkImage, uint32_t>, std::pair<uint32_t, uint32_t>>> images;
        std::vector<std::pair<std::unordered_map<VkBuffer, uint32_t>, std::pair<uint32_t, uint32_t>>> buffers;
        VkDeviceMemory buffersMemory { nullptr };
        VkDeviceMemory imagesMemory { nullptr };
        window::window *window { nullptr };
        queueSet queuesSet;
        DeviceDescription *description { nullptr };
    };

    namespace tools
    {
        inline DeviceType VkDevTypeToEngineDevType( VkPhysicalDeviceType type );
        uint32_t requeredMemoryTypeIndex( types::device device, uint32_t type, VkMemoryPropertyFlags properties );
        uint32_t requeredMemoryTypeIndex( VkPhysicalDeviceMemoryProperties memProperties, uint32_t type, VkMemoryPropertyFlags properties );
    } // namespace tools
} // namespace Engine
#endif