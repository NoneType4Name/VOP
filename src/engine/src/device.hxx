#pragma once
#ifndef DEVICE_HXX
#    define DEVICE_HXX
#    include <platform.hxx>
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <descriptorPool.hxx>
#    include <surface.hxx>
#    include <queue.hxx>
#    include <buffer.hxx>

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
        void addImagesMemorySize( uint32_t index, uint32_t size );
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
        window::window *window { nullptr };
        queueSet queuesSet;
        DeviceDescription *description { nullptr };
        friend buffer;

      private:
        uint32_t setImageMemory( VkImage image, VkMemoryPropertyFlags properties );
        void resetImageMemory( VkImage image, uint32_t index );
        std::vector<std::pair<std::unordered_map<VkImage, uint32_t>, std::pair<uint32_t, uint32_t>>> images;
        std::unordered_map<VkImage, uint32_t> imageMemoryIndecies;
        VkDeviceMemory imagesMemory { nullptr };
        std::vector<std::pair<std::unordered_map<VkBuffer, uint32_t>, std::pair<uint32_t, uint32_t>>> buffers;
        std::unordered_map<VkBuffer, uint32_t> bufferMemoryIndecies;
        VkDeviceMemory buffersMemory { nullptr };
        void allocateBufferMemory( VkBuffer buffer, VkMemoryPropertyFlags properties );
        void mapBufferMemory( VkBuffer buffer, );
        void freeBufferMemory( VkBuffer buffer );
    };

    namespace tools
    {
        inline DeviceType VkDevTypeToEngineDevType( VkPhysicalDeviceType type );
        uint32_t requeredMemoryTypeIndex( types::device device, uint32_t type, VkMemoryPropertyFlags properties );
        uint32_t requeredMemoryTypeIndex( VkPhysicalDeviceMemoryProperties memProperties, uint32_t type, VkMemoryPropertyFlags properties );
    } // namespace tools
} // namespace Engine
#endif