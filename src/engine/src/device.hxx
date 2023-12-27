#pragma once
#ifndef DEVICE_HXX
#    define DEVICE_HXX
#    include <platform.hxx>
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <surface.hxx>
#    include <queue.hxx>
#    include <buffer.hxx>

namespace Engine
{
    struct DeviceDescription::DATA_TYPE
    {
        DATA_TYPE() = default;
        DATA_TYPE( types::DeviceDescription parent ) :
            parent { parent } {}
        void init( VkPhysicalDevice device );
        std::vector<VkQueueFamilyProperties> queueFamilyProperties;
        VkPhysicalDeviceMemoryProperties memProperties {};
        VkPhysicalDeviceProperties properties {};
        VkPhysicalDeviceFeatures features {};
        VkPhysicalDevice phDevice { nullptr };
        types::DeviceDescription parent { nullptr };
    };

    struct device::DATA_TYPE
    {
        DATA_TYPE( device *parent, types::DeviceDescription description );
        // void addImagesMemorySize( uint32_t index, uint32_t size );
        // void addBuffersMemorySize( uint32_t index, uint32_t size );
        void create( VkDeviceCreateInfo createInfo );
        VkFormat formatPriority( const std::vector<VkFormat> &formats, VkImageTiling ImageTiling, VkFormatFeatureFlags FormatFeatureFlags );
        types::swapchain regSwapchain( types::swapchain swapchain );
        uint32_t requeredMemoryTypeIndex( uint32_t type, VkMemoryPropertyFlags properties );
        ~DATA_TYPE();
        VkDevice handle { nullptr };
        VkCommandPool grapchicPool { nullptr };
        VkCommandPool transferPool { nullptr };
        VkCommandPool presentPool { nullptr };
        std::vector<const char *> extensions;
        std::unordered_set<std::shared_ptr<swapchain>> swapchains;
        // std::vector<std::unique_ptr<descriptorPool>> descriptorPools;
        // std::vector<std::unique_ptr<shader>> shaders;
        // std::vector<std::unique_ptr<layout>> layouts;
        // std::vector<std::unique_ptr<pipeline>> pipelines;
        // memoryTypeIndex<<handle, offset>, <size, oversize>>;
        window::types::window window { nullptr };
        queueSet queuesSet;
        types::DeviceDescription description { nullptr };
        types::device parent { nullptr };

      private:
        void setExtensions( std::vector<const char *> &deviceExtensions );
        bool supportExtensions();
        // uint32_t setImageMemory( VkImage image, VkMemoryPropertyFlags properties );
        // void resetImageMemory( VkImage image, uint32_t index );
        // std::vector<std::pair<std::unordered_map<VkImage, uint32_t>, std::pair<uint32_t, uint32_t>>> images;
        // std::unordered_map<VkImage, uint32_t> imageMemoryIndecies;
        // VkDeviceMemory imagesMemory { nullptr };
        // std::vector<std::pair<std::unordered_map<VkBuffer, uint32_t>, std::pair<uint32_t, uint32_t>>> buffers;
        // std::unordered_map<VkBuffer, uint32_t> bufferMemoryIndecies;
        // VkDeviceMemory buffersMemory { nullptr };
        // void allocateBufferMemory( VkBuffer buffer, VkMemoryPropertyFlags properties );
        // void writeBufferMemory( VkBuffer buffer, VkMemoryMapFlags flags, void **data, VkDeviceSize size );
        // void freeBufferMemory( VkBuffer buffer );
        friend class buffer;
    };

    namespace tools
    {
        inline DeviceType VkDevTypeToEngineDevType( VkPhysicalDeviceType type );
    } // namespace tools
} // namespace Engine
#endif