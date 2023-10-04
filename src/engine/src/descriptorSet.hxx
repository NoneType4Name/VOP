#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <engine.hxx>

namespace Engine
{
    namespace tools
    {
        typedef uint64_t descriptorSetID;
        class descriptorSet
        {
          public:
            descriptorSet() = default;
            descriptorSet( std::vector<VkDescriptorSetLayoutBinding> layouts, VkDescriptorPool pool = nullptr );
            VkDescriptorSetLayout getLayout() const;
            VkDescriptorSet getHandle() const;
            descriptorSetID getID() const;
            std::vector<VkDescriptorPoolSize> getSizes() const;
            void create( VkDescriptorPool pool );
            ~descriptorSet();

          private:
            descriptorSetID id{ 0 };
            std::vector<VkDescriptorSetLayoutBinding> layoutBinds;
            VkDescriptorPool fromPool{ nullptr };
            VkDescriptorSetLayout layout{ nullptr };
            VkDescriptorSet set{ nullptr };
        };

        void createDescriptorPool();
        void destroyDescriptorPool();
        const descriptorSet &getDescriptorSet( descriptorSetID id );
    } // namespace tools
} // namespace Engine