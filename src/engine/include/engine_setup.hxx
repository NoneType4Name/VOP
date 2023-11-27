#pragma once
#ifndef ENGINE_SETUP_HXX
#    define ENGINE_SETUP_HXX
#    include <engine.hxx>
#    include <EHI.hxx>
#    include <platform.hxx>

namespace Engine
{
    class ENGINE_EXPORT InstanceSetup
    {
      public:
        virtual void instanceLayers( types::instance instance, std::vector<const char *> &rLayers );
        virtual void instanceExtensions( types::instance instance, std::vector<const char *> &rExtensions );
        virtual void instanceNextChain( types::instance instace, const void *&pNext, std::vector<void *> &dataPointers );
        virtual void deviceNextChain( types::device device, const void *&pNext, std::vector<void *> &dataPointers, void *userPoiner );
        virtual void deviceExtensions( types::device device, std::vector<const char *> &deviceExtensions, void *userPoiner );
        virtual void deviceFeatures( types::device device, VkPhysicalDeviceFeatures &features, void *userPoiner );
        virtual void deviceQueueSet( types::device device, queueSet &queues, VkSurfaceKHR surface, void *userPoiner );
        virtual void deviceDescriptors( types::device device, VkDevice pDevice, std::vector<descriptorPool> &descriptorSets );
        virtual void queueSetNextChain( queueSet *queueSet, const void *&pNext, std::vector<void *> &dataPointers );
        virtual void queueSetFlags( queueSet *queueSet, VkDeviceQueueCreateFlags &flag );
        virtual void renderpass( types::pass renderpass, types::link link, VkRenderPassCreateInfo &createInfo, std::vector<void *> &dataPointer, void *userPointer );
        virtual void surfaceNextChain( window::types::window window, const void *&pNext, std::vector<void *> &dataPointers, void *userPoiner );
        virtual void surfaceFlags( window::types::window window, VkWin32SurfaceCreateFlagsKHR flags, void *userPoiner );
        virtual void swapchain( types::link swapchain, VkSwapchainCreateInfoKHR &createInfo, std::vector<void *> &dataPointer, void *userPoiner );
    };
} // namespace Engine
#endif