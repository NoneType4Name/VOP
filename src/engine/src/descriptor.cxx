#include <descriptor.hxx>
#include <device.hxx>

namespace Engine
{
    namespace tools
    {
        descriptor::descriptor( std::vector<VkDescriptorSetLayoutBinding> layouts )
        {
            std::vector<VkDescriptorPoolSize> sizes{};
            sizes.reserve( layouts.size() );

            VkDescriptorSetLayoutCreateInfo layoutsSet{};
            layoutsSet.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutsSet.bindingCount = layouts.size();
            layoutsSet.pBindings    = layouts.data();
            uint32_t maxSets{ 0 };
            for( auto l : layouts )
            {
                sizes.push_back( { l.descriptorType, l.descriptorCount } );
                if( l.descriptorCount > maxSets ) maxSets = l.descriptorCount;
            }
            vkCreateDescriptorSetLayout( getDevice(), &layoutsSet, ALLOCATION_CALLBACK, &layout );

            VkDescriptorPoolCreateInfo poolCreateInfo{};
            poolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolCreateInfo.poolSizeCount = sizes.size();
            poolCreateInfo.pPoolSizes    = sizes.data();
            poolCreateInfo.maxSets       = maxSets;
            vkCreateDescriptorPool( getDevice(), &poolCreateInfo, ALLOCATION_CALLBACK, &pool ); // todo ads class

            VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
            descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            descriptorSetAllocateInfo.descriptorSetCount = 1;
            descriptorSetAllocateInfo.pSetLayouts        = &layout;
            vkAllocateDescriptorSets( getDevice(), &descriptorSetAllocateInfo, &set );
        }

        VkDescriptorPool descriptor::getPool() const
        {
            return pool;
        }

        VkDescriptorSetLayout descriptor::getLayout() const
        {
            return layout;
        }

        VkDescriptorSet descriptor::getSet() const
        {
            return set;
        }

        descriptor::~descriptor()
        {
            vkFreeDescriptorSets( getDevice(), pool, 1, &set );
            vkDestroyDescriptorPool( getDevice(), pool, ALLOCATION_CALLBACK ); // todo as classs
            vkDestroyDescriptorSetLayout( getDevice(), layout, ALLOCATION_CALLBACK );
        }

    } // namespace tools
} // namespace Engine