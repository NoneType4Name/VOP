#pragma once
#ifndef DEVICE_HXX
#    define DEVICE_HXX
#    include <ObjectiveVulkan/common/globals.hxx>
#    include <ObjectiveVulkan/common/logging.hxx>
#    include <ObjectiveVulkan/surface.hxx>
#    include <ObjectiveVulkan/buffer.hxx>
#    include <ObjectiveVulkan/image.hxx>
#    include <ObjectiveVulkan/queue.hxx>

namespace Engine
{
    struct deviceDescription::OBJECTIVE_VULKAN_DATA_TYPE final
    {
        OBJECTIVE_VULKAN_DATA_TYPE() = delete;
        OBJECTIVE_VULKAN_DATA_TYPE( types::deviceDescription parent, struct instance *instance, VkPhysicalDevice device );
        ~OBJECTIVE_VULKAN_DATA_TYPE();
        std::vector<VkQueueFamilyProperties> queueFamilyProperties;
        VkPhysicalDeviceMemoryProperties memProperties {};
        VkPhysicalDeviceProperties properties {};
        VkPhysicalDeviceFeatures features {};
        VkPhysicalDevice phDevice { nullptr };
        instance *instance { nullptr };
        types::deviceDescription parent;
    };

    struct device::OBJECTIVE_VULKAN_DATA_TYPE final
    {
        OBJECTIVE_VULKAN_DATA_TYPE() = delete;
        OBJECTIVE_VULKAN_DATA_TYPE( types::device parent, types::deviceDescription description );
        ~OBJECTIVE_VULKAN_DATA_TYPE();
        void create( VkDeviceCreateInfo createInfo );
        types::queue getQueue( uint32_t familyIndex, uint32_t index );
        std::vector<const char *> extensions;
        std::set<types::renderPass> renderpasses;
        std::set<types::swapchain> swapchains;
        std::set<types::image> images;
        std::set<types::buffer> buffers;
        std::set<types::queue> queues;
        std::unordered_map<std::string, VkShaderModule> shaders;
        types::deviceDescription description { nullptr };
        types::device parent;

      private:
        friend device;
        std::vector<std::vector<std::pair<float, types::queue>>> _queues;
        void setExtensions( std::vector<const char *> &deviceExtensions );
        bool supportExtensions();
    };
} // namespace Engine
#endif