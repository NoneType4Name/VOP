#include <descriptorSet.hxx>
#include <RHI.hxx>
#include <device.hxx>

namespace Engine
{
    namespace tools
    {
        namespace
        {
            std::unordered_map<descriptorSetID, descriptorSet> _descriptorSets{};
            descriptorSetID descriptorSet_id{ 0 };
        } // namespace
        descriptorSet::descriptorSet( std::vector<VkDescriptorSetLayoutBinding> layouts ) : id{ ++descriptorSet_id }
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

        VkDescriptorSetLayout descriptorSet::getLayout() const
        {
            return layout;
        }

        VkDescriptorSet descriptorSet::getHandle() const
        {
            return set;
        }

        descriptorSetID descriptorSet::getID() const
        {
            return id;
        };

        descriptorSet::~descriptorSet()
        {
            vkFreeDescriptorSets( getDevice(), pool, 1, &set );
            vkDestroyDescriptorPool( getDevice(), pool, ALLOCATION_CALLBACK ); // todo as classs
            vkDestroyDescriptorSetLayout( getDevice(), layout, ALLOCATION_CALLBACK );
        }

        void createDescriptorPool()
        {
        }

        void destroyDescriptorPool()
        {
        }
    } // namespace tools
} // namespace Engine