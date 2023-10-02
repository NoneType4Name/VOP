#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>

namespace Engine
{
    namespace tools
    {
        typedef uint64_t descriptorSetID;
        class descriptorSet
        {
          public:
            descriptorSet() = default;
            descriptorSet( std::vector<VkDescriptorSetLayoutBinding> layouts );
            VkDescriptorPool getPool() const;
            VkDescriptorSetLayout getLayout() const;
            VkDescriptorSet getHandle() const;
            descriptorSetID getID() const;
            ~descriptorSet();

          private:
            descriptorSetID id;
            VkDescriptorPool pool{ nullptr };
            VkDescriptorSetLayout layout{ nullptr };
            VkDescriptorSet set{ nullptr };
        };
    } // namespace tools
} // namespace Engine