#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>

namespace Engine
{
    namespace tools
    {
        class descriptor
        {
          public:
            descriptor() = default;
            descriptor( std::vector<VkDescriptorSetLayoutBinding> layouts );
            VkDescriptorPool getPool() const;
            VkDescriptorSetLayout getLayout() const;
            VkDescriptorSet getSet() const;
            ~descriptor();

          private:
            VkDescriptorPool pool{ nullptr };
            VkDescriptorSetLayout layout{ nullptr };
            VkDescriptorSet set{ nullptr };
        };
    } // namespace tools
} // namespace Engine