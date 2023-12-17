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
        InstanceSetup();
        ~InstanceSetup();
        virtual void instanceLayers( types::instance instance, std::vector<const char *> &rLayers, void *userPoiner );
        virtual void instanceExtensions( types::instance instance, std::vector<const char *> &rExtensions, void *userPoiner );
        virtual void instanceNextChain( types::instance instace, const void *&pNext, std::vector<void *> &dataPointers, void *userPoiner );
        virtual void instanceNextChainClear( types::instance instace, std::vector<void *> &dataPointers, void *userPoiner );
        virtual void deviceNextChain( types::device device, const void *&pNext, std::vector<void *> &dataPointers, void *userPoiner );
        virtual void deviceNextChainClear( types::device device, std::vector<void *> &dataPointers, void *userPoiner );
        virtual void deviceExtensions( types::device device, std::vector<const char *> &deviceExtensions, void *userPoiner );
        virtual void deviceFeatures( types::device device, VkPhysicalDeviceFeatures &features, void *userPoiner );
        virtual void deviceQueueSet( types::device device, queueSet &queues, VkSurfaceKHR surface, void *userPoiner );
        virtual void queueFlags( queueSet *queueSet, VkDeviceQueueCreateFlags &flags, void *userPoiner );
        virtual void renderpassInfo( types::pass renderpass, types::link link, VkRenderPassCreateInfo &createInfo, std::vector<void *> &dataPointer, void *userPointer );
        virtual void renderpassInfoClear( types::pass renderpass, types::link link, std::vector<void *> &dataPointer, void *userPointer );
        virtual void surfaceInfo( window::types::window window, const void *&pNext, VkFlags &flags, std::vector<void *> &dataPointers, void *userPoiner );
        virtual void surfaceInfoClear( window::types::window window, std::vector<void *> &dataPointers, void *userPoiner );
        virtual void swapchainInfo( types::link swapchain, VkSwapchainCreateInfoKHR &createInfo, std::vector<void *> &dataPointer, void *userPoiner );
        virtual void swapchainInfoClear( types::link swapchain, std::vector<void *> &dataPointer, void *userPoiner );
        // virtual void descriptorPoolInfo( types::descriptorPool pool, descriptorPool::SetOfBindingsInfo &sets, void *userData, void *userPoiner );
        // virtual void descriptorPoolInfoClear( types::descriptorPool pool, void *userData, void *userPoiner );
        virtual void modelVertexInputBindingDescription( types::model model, VkVertexInputBindingDescription &description );
        virtual void modelVertexInputAttributeDescription( types::model model, std::vector<VkVertexInputAttributeDescription> &descriptions );
        virtual void modelDescriptorSetLayout( types::model model, descriptorPool::SetOfBindingsInfo &sets );
        virtual void layoutInfo( types::layout layout, const void *&pNext, VkPipelineLayoutCreateFlags &flags, std::vector<void *> &dataPointer, void *userPoiner );
        virtual void layoutInfoClear( types::layout layout, std::vector<void *> &dataPointer, void *userPoiner );
        virtual void pipelineInfo( types::pipeline pipeline, VkGraphicsPipelineCreateInfo &createInfo, std::vector<void *> &dataPointer, void *userPoiner );
        virtual void pipelineInfoClear( types::pipeline pipeline, std::vector<void *> &dataPointer, void *userPoiner );
        virtual void shader( types::shader shader, VkPipelineShaderStageCreateInfo &stageCreateInfo, void *userPoiner );
    };
} // namespace Engine
#endif