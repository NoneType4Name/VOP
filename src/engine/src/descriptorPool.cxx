#include <descriptorPool.hxx>
#include <device.hxx>
#include <EHI.hxx>

namespace Engine
{
    void InstanceSetup::descriptorPoolInfo( types::descriptorPool pool, descriptorPool::SetOfBindingsInfo &sets, void *userData, void *userPoiner )
    {
        sets.resize( 1 );
        sets.back().resize( 2, {} );
        sets.back()[ 0 ].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        sets.back()[ 0 ].binding         = 0;
        sets.back()[ 0 ].dstArrayElement = 0;
        sets.back()[ 0 ].descriptorCount = 1;
        sets.back()[ 0 ].stageFlags      = VK_SHADER_STAGE_ALL;
        sets.back()[ 0 ].pBufferInfo     = nullptr;

        sets.back()[ 1 ].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        sets.back()[ 1 ].binding         = 1;
        sets.back()[ 1 ].dstArrayElement = 0;
        sets.back()[ 1 ].descriptorCount = 1;
        sets.back()[ 1 ].stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;
        sets.back()[ 1 ].pImageInfo      = nullptr;
    }
    void InstanceSetup::descriptorPoolInfoClear( types::descriptorPool pool, void *dataPointer, void *userPoiner ) {}

    descriptorPool::descriptorPool( types::device device, void *userData )
    {
        DEFINE_DATA_FIELD
        data->device = device;
        SetOfBindingsInfo sets {};
        data->device->data->window->data->instance->data->setup->descriptorPoolInfo( this, sets, userData, data->device->data->window->data->instance->data->userPointer );
        data->sets.resize( sets.size() );
        data->layouts.reserve( sets.size() );

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
        vkCreateDescriptorPool( data->device->data->device, &poolCreateInfo, ALLOCATION_CALLBACK, &data->handle );
        data->device->data->window->data->instance->data->setup->descriptorPoolInfoClear( this, userData, data->device->data->window->data->instance->data->userPointer );

        for ( const auto &bindings : sets )
        {
            data->layouts.emplace_back( nullptr );
            std::vector<VkDescriptorSetLayoutBinding> binds;
            binds.reserve( bindings.size() );
            for ( const auto &info : bindings )
                binds.push_back( { info.binding, info.descriptorType, info.descriptorCount, info.stageFlags, info.pImmutableSamplers } );
            VkDescriptorSetLayoutCreateInfo layoutsSet {};
            layoutsSet.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutsSet.bindingCount = binds.size();
            layoutsSet.pBindings    = binds.data();
            CHECK_RESULT( vkCreateDescriptorSetLayout( data->device->data->device, &layoutsSet, ALLOCATION_CALLBACK, &data->layouts.back() ) );
        }
        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo {};
        descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorSetCount = data->layouts.size();
        descriptorSetAllocateInfo.pSetLayouts        = data->layouts.data();
        descriptorSetAllocateInfo.descriptorPool     = data->handle;
        CHECK_RESULT( vkAllocateDescriptorSets( data->device->data->device, &descriptorSetAllocateInfo, data->sets.data() ) );
        std::vector<VkWriteDescriptorSet> wds;
        wds.reserve( sets.size() );
        size_t set_index { 0 };
        for ( const auto &set : sets )
        {
            for ( const auto &bind : set )
                wds.push_back( { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, bind.pNext, data->sets[ set_index ], bind.binding, bind.dstArrayElement, bind.descriptorCount, bind.descriptorType, bind.pImageInfo, bind.pBufferInfo, bind.pTexelBufferView } );
            ++set_index;
        }
        // vkUpdateDescriptorSets( data->device->data->device, wds.size(), wds.data(), 0, nullptr );
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
        for ( const auto layout : data->layouts )
            vkDestroyDescriptorSetLayout( data->device->data->device, layout, ALLOCATION_CALLBACK );
        vkFreeDescriptorSets( data->device->data->device, data->handle, data->sets.size(), data->sets.data() );
        vkDestroyDescriptorPool( data->device->data->device, data->handle, ALLOCATION_CALLBACK );
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