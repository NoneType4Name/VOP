#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>

namespace Engine
{
    class descriptorPool
    {
      public:
        descriptorPool( VkDevice device, std::vector<std::vector<VkDescriptorSetLayoutBinding>> sets );
        ~descriptorPool();

        std::vector<VkDescriptorSetLayout> layouts;
        std::vector<VkDescriptorSet> sets;
        VkDevice device { nullptr };
        VkDescriptorPool handle { nullptr };
    };
} // namespace Engine