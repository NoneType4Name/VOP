#include <descriptorPool.hxx>
#include <device.hxx>

namespace Engine
{
    descriptorPool::descriptorPool( VkDevice device, std::vector<std::vector<VkDescriptorSetLayoutBinding>> sets )
    {
        this->device = device;
        this->sets.resize( sets.size() );
        this->layouts.reserve( sets.size() );

        uint32_t max { 0 };
        std::vector<VkDescriptorPoolSize> sizes;
        sizes.reserve( sets.size() );
        for ( const auto &binds : sets )
        {
            for ( const auto &bind : binds )
            {
                sizes.push_back( { bind.descriptorType, bind.descriptorCount } );
                if ( max < bind.descriptorCount )
                    max = bind.descriptorCount;
            }
        }

        VkDescriptorPoolCreateInfo poolCreateInfo {};
        poolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolCreateInfo.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolCreateInfo.poolSizeCount = sizes.size();
        poolCreateInfo.pPoolSizes    = sizes.data();
        poolCreateInfo.maxSets       = max;
        vkCreateDescriptorPool( device, &poolCreateInfo, ALLOCATION_CALLBACK, &handle );

        for ( const auto &bindings : sets )
        {
            this->layouts.emplace_back( nullptr );
            VkDescriptorSetLayoutCreateInfo layoutsSet {};
            layoutsSet.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutsSet.bindingCount = bindings.size();
            layoutsSet.pBindings    = bindings.data();
            CHECK_RESULT( vkCreateDescriptorSetLayout( this->device, &layoutsSet, ALLOCATION_CALLBACK, &this->layouts.back() ) );
        }
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo {};
        descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorSetCount = this->layouts.size();
        descriptorSetAllocateInfo.pSetLayouts        = this->layouts.data();
        descriptorSetAllocateInfo.descriptorPool     = handle;
        CHECK_RESULT( vkAllocateDescriptorSets( device, &descriptorSetAllocateInfo, this->sets.data() ) );
    }

    // std::vector<VkDescriptorPoolSize> descriptorPool::getSizes()
    // {
    //     std::vector<VkDescriptorPoolSize> sizes;
    //     sizes.reserve( layoutBinds.size() );
    //     for ( auto l : layoutBinds )
    //     {
    //         sizes.push_back( { l.descriptorType, l.descriptorCount } );
    //     }
    //     return sizes;
    // }

    descriptorPool::~descriptorPool()
    {
        for ( const auto layout : layouts )
            vkDestroyDescriptorSetLayout( device, layout, ALLOCATION_CALLBACK );
        vkFreeDescriptorSets( device, handle, sets.size(), sets.data() );
        vkDestroyDescriptorPool( device, handle, ALLOCATION_CALLBACK );
    }

    // void createDescriptorPool()
    // {
    //     uint8_t maxSets { 0 };
    //     std::vector<VkDescriptorPoolSize> sizes;
    //     for ( auto &size : _descriptorSets )
    //     {
    //         for ( auto &sz : size.second->getSizes() )
    //         {
    //             sizes.push_back( sz );
    //             if ( sz.descriptorCount > maxSets ) maxSets = sz.descriptorCount;
    //         }
    //     }

    //     VkDescriptorPoolCreateInfo poolCreateInfo {};
    //     poolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    //     poolCreateInfo.poolSizeCount = sizes.size();
    //     poolCreateInfo.pPoolSizes    = sizes.data();
    //     poolCreateInfo.maxSets       = maxSets;
    //     vkCreateDescriptorPool( getDevice(), &poolCreateInfo, ALLOCATION_CALLBACK, &_pool );
    //     for ( auto &set : _descriptorSets )
    //     {
    //         set.second->init( _pool );
    //     }
    // }

    // void destroyDescriptorPool()
    // {
    //     for ( auto &set : _descriptorSets )
    //     {
    //         delete set.second;
    //     }
    //     vkDestroyDescriptorPool( getDevice(), _pool, ALLOCATION_CALLBACK );
    // }

    // descriptorPool *getDescriptorSet( descriptorSetID id )
    // {
    //     return _descriptorSets[ id ];
    // }

    // size_t getDescriptorSets( std::vector<descriptorPool *> &descriptorSets )
    // {
    //     descriptorSets.reserve( _descriptorSets.size() );
    //     for ( auto &pair : _descriptorSets )
    //     {
    //         descriptorSets.push_back( pair.second );
    //     }
    //     return descriptorSets.size();
    // }
} // namespace Engine