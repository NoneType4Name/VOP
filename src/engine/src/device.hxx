#pragma once
#ifndef DEVICE_HXX
#    define DEVICE_HXX
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <platform.hxx>
#    include <surface.hxx>
#    include <buffer.hxx>
#    include <image.hxx>
#    include <queue.hxx>

namespace Engine
{
    struct deviceDescription::DATA_TYPE final
    {
        DATA_TYPE() = delete;
        DATA_TYPE( types::deviceDescription parent, struct instance *instance, VkPhysicalDevice device );
        ~DATA_TYPE();
        std::vector<VkQueueFamilyProperties> queueFamilyProperties;
        VkPhysicalDeviceMemoryProperties memProperties {};
        VkPhysicalDeviceProperties properties {};
        VkPhysicalDeviceFeatures features {};
        VkPhysicalDevice phDevice { nullptr };
        instance *instance { nullptr };
        types::deviceDescription parent;
    };

    struct device::DATA_TYPE final
    {
        DATA_TYPE() = delete;
        DATA_TYPE( types::device parent, types::deviceDescription description );
        ~DATA_TYPE();
        void create( VkDeviceCreateInfo createInfo );
        types::queue getQueue( uint32_t familyIndex, uint32_t index );
        std::vector<const char *> extensions;
        std::set<types::renderPass> renderpasses;
        std::set<types::swapchain> swapchains;
        std::set<types::image> images;
        std::set<types::buffer> buffers;
        std::set<types::queue> queues;
        // std::vector<std::unique_ptr<descriptorPool>> descriptorPools;
        // std::vector<std::unique_ptr<shader>> shaders;
        // std::vector<std::unique_ptr<layout>> layouts;
        // std::vector<std::unique_ptr<pipeline>> pipelines;
        // memoryTypeIndex<<handle, offset>, <size, oversize>>;
        types::deviceDescription description { nullptr };
        types::device parent;

      private:
        friend device;
        std::vector<std::vector<std::pair<float, types::queue>>> _queues;
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
    };
} // namespace Engine
#endif