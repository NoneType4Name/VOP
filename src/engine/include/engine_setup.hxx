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
        virtual void queueSetInfo( queueSet *queueSet, VkDeviceQueueCreateFlags &flags, const void *&pNext, std::vector<void *> &dataPointers, void *userPoiner );
        virtual void queueSetNextChainClear( queueSet *queueSet, std::vector<void *> &dataPointers, void *userPoiner );
        virtual void renderpassInfo( types::pass renderpass, types::link link, VkRenderPassCreateInfo &createInfo, std::vector<void *> &dataPointer, void *userPointer );
        virtual void renderpassInfoClear( types::pass renderpass, types::link link, std::vector<void *> &dataPointer, void *userPointer );
        virtual void surfaceInfo( window::types::window window, const void *&pNext, VkFlags &flags, std::vector<void *> &dataPointers, void *userPoiner );
        virtual void surfaceInfoClear( window::types::window window, std::vector<void *> &dataPointers, void *userPoiner );
        virtual void swapchainInfo( types::link swapchain, VkSwapchainCreateInfoKHR &createInfo, std::vector<void *> &dataPointer, void *userPoiner );
        virtual void swapchainInfoClear( types::link swapchain, std::vector<void *> &dataPointer, void *userPoiner );
        virtual void descriptorPoolInfo( types::descriptorPool pool, descriptorPool::SetOfBindingsInfo &sets, void *dataPointer, void *userPoiner );
        virtual void descriptorPoolInfoClear( types::descriptorPool pool, void *dataPointer, void *userPoiner );

        virtual void layoutNextChain( types::layout layout, const void *&pNext, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner );
        virtual void layoutFlags( types::layout layout, VkPipelineLayoutCreateFlags &flags, void *userPoiner );
        virtual void pipelineFlags( types::pipeline pipeline, VkPipelineCreateFlags &flags, void *userPoiner );
        virtual void pipelineNextChain( types::pipeline pipeline, const void *&pNext, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner );
        virtual void pipelineVertexInputState( types::pipeline pipeline, VkPipelineVertexInputStateCreateInfo &stageInfo, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner );
        virtual void pipelineInputAssemblyState( types::pipeline pipeline, VkPipelineInputAssemblyStateCreateInfo &stageInfo, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner );
        virtual void pipelineTessellationState( types::pipeline pipeline, VkPipelineTessellationStateCreateInfo &stageInfo, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner );
        virtual void pipelineViewportState( types::pipeline pipeline, VkPipelineViewportStateCreateInfo &stageInfo, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner );
        virtual void pipelineRasterizationState( types::pipeline pipeline, VkPipelineRasterizationStateCreateInfo &stageInfo, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner );
        virtual void pipelineMultisampleState( types::pipeline pipeline, VkPipelineMultisampleStateCreateInfo &stageInfo, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner );
        virtual void pipelineDepthStencilState( types::pipeline pipeline, VkPipelineDepthStencilStateCreateInfo &stageInfo, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner );
        virtual void pipelineColorBlendState( types::pipeline pipeline, VkPipelineColorBlendStateCreateInfo &stageInfo, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner );
        virtual void pipelineDynamicState( types::pipeline pipeline, VkPipelineDynamicStateCreateInfo &stageInfo, std::vector<std::unique_ptr<void>> &dataPointer, void *userPoiner );
        virtual void pipelineRenderSubpass( types::pipeline pipeline, uint32_t &subpass, void *userPoiner );
        virtual void pipelineBaseIndex( types::pipeline pipeline, int32_t &basePipelineIndex, void *userPoiner );
        virtual void shader( types::shader shader, VkPipelineShaderStageCreateInfo &stageCreateInfo, void *userPoiner );
    };
} // namespace Engine
#endif