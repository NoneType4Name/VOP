#include <descriptorSet.hxx>
#include <RHI.hxx>
#include <device.hxx>

namespace Engine
{
    namespace tools
    {
        namespace
        {
            std::unordered_map<descriptorSetID, descriptorSet *> _descriptorSets{};
            descriptorSetID descriptorSet_id{ 0 };
            VkDescriptorPool _pool{ nullptr };
        } // namespace

        descriptorSet::descriptorSet( std::vector<VkDescriptorSetLayoutBinding> layouts, VkDescriptorPool pool ) : id{ ++descriptorSet_id }
        {
            layoutBinds.assign( layouts.begin(), layouts.end() );
            if( pool )
                init( pool );
            _descriptorSets[ id ] = this;
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

        std::vector<VkDescriptorPoolSize> descriptorSet::getSizes() const
        {
            std::vector<VkDescriptorPoolSize> sizes;
            sizes.reserve( layoutBinds.size() );
            for( auto l : layoutBinds )
            {
                sizes.push_back( { l.descriptorType, l.descriptorCount } );
            }
            return sizes;
        }

        void descriptorSet::init( VkDescriptorPool pool )
        {
            fromPool = pool;
            VkDescriptorSetLayoutCreateInfo layoutsSet{};
            layoutsSet.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutsSet.bindingCount = layoutBinds.size();
            layoutsSet.pBindings    = layoutBinds.data();
            vkCreateDescriptorSetLayout( getDevice(), &layoutsSet, ALLOCATION_CALLBACK, &layout );

            VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
            descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            descriptorSetAllocateInfo.descriptorSetCount = 1;
            descriptorSetAllocateInfo.pSetLayouts        = &layout;
            descriptorSetAllocateInfo.descriptorPool     = pool;
            vkAllocateDescriptorSets( getDevice(), &descriptorSetAllocateInfo, &set );
        }

        descriptorSet::~descriptorSet()
        {
            vkDestroyDescriptorSetLayout( getDevice(), layout, ALLOCATION_CALLBACK );
            _descriptorSets[ id ] = nullptr;
        }

        void createDescriptorPool()
        {
            uint8_t maxSets{ 0 };
            std::vector<VkDescriptorPoolSize> sizes;
            for( auto &size : _descriptorSets )
            {
                for( auto &sz : size.second->getSizes() )
                {
                    sizes.push_back( sz );
                    if( sz.descriptorCount > maxSets ) maxSets = sz.descriptorCount;
                }
            }

            VkDescriptorPoolCreateInfo poolCreateInfo{};
            poolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolCreateInfo.poolSizeCount = sizes.size();
            poolCreateInfo.pPoolSizes    = sizes.data();
            poolCreateInfo.maxSets       = maxSets;
            vkCreateDescriptorPool( getDevice(), &poolCreateInfo, ALLOCATION_CALLBACK, &_pool );
        }

        void destroyDescriptorPool()
        {
            vkDestroyDescriptorPool( getDevice(), _pool, ALLOCATION_CALLBACK );
        }

        void createDescriptorSets()
        {

            for( auto &set : _descriptorSets )
            {
                set.second->init( _pool );
            }
        }

        void destroyDescriptorSets()
        {
            for( auto &set : _descriptorSets )
            {
                delete set.second;
            }
        }

        descriptorSet *getDescriptorSet( descriptorSetID id )
        {
            return _descriptorSets[ id ];
        }
    } // namespace tools
} // namespace Engine