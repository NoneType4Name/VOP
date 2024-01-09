#include <device.hxx>
#include <surface.hxx>
#include <swapchain.hxx>
#include <EHI.hxx>

namespace Engine
{
    // void InstanceSetup::deviceDescriptorPools( types::device device, std::vector<std::unique_ptr<Engine::descriptorPool>> &descriptorSets )
    // {
    //     descriptorPool::SetOfBindingsInfo set( 1 );
    //     set.back().resize( 2, {} );
    //     set.back()[ 0 ].descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    //     set.back()[ 0 ].binding         = 0;
    //     set.back()[ 0 ].dstArrayElement = 0;
    //     set.back()[ 0 ].descriptorCount = 1;
    //     set.back()[ 0 ].stageFlags      = VK_SHADER_STAGE_ALL;
    //     set.back()[ 0 ].pBufferInfo     = nullptr;

    //     set.back()[ 1 ].descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    //     set.back()[ 1 ].binding         = 1;
    //     set.back()[ 1 ].dstArrayElement = 0;
    //     set.back()[ 1 ].descriptorCount = 1;
    //     set.back()[ 1 ].stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;
    //     set.back()[ 1 ].pImageInfo      = nullptr;
    //     descriptorSets.emplace_back( pDevice, set );
    // }
    DeviceDescription::DeviceDescription( instance *instance, VkPhysicalDevice phDevice )
    {
        DEFINE_DATA_FIELD( instance, phDevice );
    }

    DeviceDescription::~DeviceDescription() {};

    device::DATA_TYPE::DATA_TYPE( types::device parent, types::DeviceDescription description ) :
        parent { parent }, description { description }, queuesSet { parent }, memory { this }
    {
    }

    device::DATA_TYPE::~DATA_TYPE()
    {
    }

    device::device( types::DeviceDescription description )
    {
        auto &_data = const_cast<std ::unique_ptr<DATA_TYPE> &>( data );
        _data.reset( new DATA_TYPE { this, description } );
        data->description = description;
        data->description->data->instance->data->devices.emplace( this );
    }

    device::~device()
    {
        if ( data->grapchicPool )
            vkDestroyCommandPool( data->handle, data->grapchicPool, ALLOCATION_CALLBACK );
        if ( data->transferPool )
            vkDestroyCommandPool( data->handle, data->transferPool, ALLOCATION_CALLBACK );
        if ( data->presentPool )
            vkDestroyCommandPool( data->handle, data->presentPool, ALLOCATION_CALLBACK );
        // data->pipelines.clear();
        // data->shaders.clear();
        // data->layouts.clear();
        // data->descriptorPools.clear();
        vkDestroyDevice( data->handle, ALLOCATION_CALLBACK );
        for ( auto &s : data->swapchains )
            delete s;
        data->description->data->instance->data->devices.erase( this );
    }

    DeviceDescription::DATA_TYPE::DATA_TYPE( types::DeviceDescription parent, struct instance *instance, VkPhysicalDevice device ) :
        parent { parent }, instance { instance }, phDevice { device }
    {
        uint32_t c;
        vkGetPhysicalDeviceQueueFamilyProperties( device, &c, nullptr );
        queueFamilyProperties.resize( c );
        vkGetPhysicalDeviceQueueFamilyProperties( device, &c, queueFamilyProperties.data() );
        vkGetPhysicalDeviceMemoryProperties( device, &memProperties );
        vkGetPhysicalDeviceProperties( device, &properties );
        vkGetPhysicalDeviceFeatures( device, &features );
        parent->name  = properties.deviceName;
        parent->type  = properties.deviceType;
        parent->grade = queueFamilyProperties.size();
    }

    DeviceDescription::DATA_TYPE::~DATA_TYPE() {}

    void device::setup( window::types::window window )
    {
        VkDeviceCreateInfo DeviceCreateInfo {};
        VkPhysicalDeviceDescriptorIndexingFeatures indexFeatures {};
        DeviceCreateInfo.pNext                                  = &indexFeatures;
        indexFeatures.sType                                     = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
        indexFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
        indexFeatures.runtimeDescriptorArray                    = VK_TRUE;
        indexFeatures.descriptorBindingVariableDescriptorCount  = VK_TRUE;
        VkPhysicalDeviceFeatures features {};
        features.samplerAnisotropy = VK_TRUE;
        features.sampleRateShading = VK_TRUE;
        if ( data->description->data->queueFamilyProperties.size() - 1 )
        {
            for ( uint32_t i { 0 }; i < data->description->data->queueFamilyProperties.size(); i++ )
            {
                VkBool32 presentSupport { false };
                vkGetPhysicalDeviceSurfaceSupportKHR( data->description->data->phDevice, i, window->data->surface, &presentSupport );
                if ( !data->queuesSet.graphic.familyIndex.has_value() && data->description->data->queueFamilyProperties[ i ].queueFlags & VK_QUEUE_GRAPHICS_BIT )
                    data->queuesSet.graphic = { i, 1.f };
                if ( !data->queuesSet.present.familyIndex.has_value() && presentSupport )
                    data->queuesSet.present = { i, 1.f };
                else if ( !data->queuesSet.transfer.familyIndex.has_value() && data->description->data->queueFamilyProperties[ i ].queueFlags & VK_QUEUE_TRANSFER_BIT )
                    data->queuesSet.transfer = { i, 1.f };
                // else if ( !data->queuesSet.compute.familyIndex.has_value() && data->description->data->queueFamilyProperties[ i ].queueFlags & VK_QUEUE_COMPUTE_BIT )
                //     data->queuesSet.compute = i;
                else
                    break;
            }
        }
        else
        {
            VkBool32 presentSupport { false };
            vkGetPhysicalDeviceSurfaceSupportKHR( data->description->data->phDevice, 0, window->data->surface, &presentSupport );
            if ( data->description->data->queueFamilyProperties[ 0 ].queueFlags & ( VK_QUEUE_TRANSFER_BIT | VK_QUEUE_GRAPHICS_BIT ) && presentSupport )
            {
                data->queuesSet = { { 0, 0, 1.f }, { 0, 0, 1.f }, { 0, 0, 1.f } };
            }
        }
        VkPhysicalDeviceFeatures2 physicalDeviceFeatures {};
        DeviceCreateInfo.pEnabledFeatures = &features;
        data->create( DeviceCreateInfo );
        VkCommandPoolCreateInfo poolCI {};
        poolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        if ( data->queuesSet.graphic.familyIndex.has_value() )
        {
            poolCI.queueFamilyIndex = data->queuesSet.graphic.familyIndex.value();
            CHECK_RESULT( vkCreateCommandPool( data->handle, &poolCI, ALLOCATION_CALLBACK, &data->grapchicPool ) );
        }
        if ( data->queuesSet.transfer.familyIndex.has_value() )
        {
            poolCI.queueFamilyIndex = data->queuesSet.transfer.familyIndex.value();
            CHECK_RESULT( vkCreateCommandPool( data->handle, &poolCI, ALLOCATION_CALLBACK, &data->transferPool ) );
        }
        if ( data->queuesSet.present.familyIndex.has_value() )
        {
            poolCI.queueFamilyIndex = data->queuesSet.present.familyIndex.value();
            CHECK_RESULT( vkCreateCommandPool( data->handle, &poolCI, ALLOCATION_CALLBACK, &data->presentPool ) );
        }
    }

    void device::setup()
    {
        VkDeviceCreateInfo DeviceCreateInfo {};
        VkPhysicalDeviceDescriptorIndexingFeatures indexFeatures {};
        DeviceCreateInfo.pNext                                  = &indexFeatures;
        indexFeatures.sType                                     = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
        indexFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
        indexFeatures.runtimeDescriptorArray                    = VK_TRUE;
        indexFeatures.descriptorBindingVariableDescriptorCount  = VK_TRUE;
        VkPhysicalDeviceFeatures features {};
        features.samplerAnisotropy        = VK_TRUE;
        features.sampleRateShading        = VK_TRUE;
        DeviceCreateInfo.pEnabledFeatures = &features;
        if ( data->description->data->queueFamilyProperties.size() - 1 )
        {
            for ( uint32_t i { 0 }; i < data->description->data->queueFamilyProperties.size(); i++ )
            {
                if ( !data->queuesSet.graphic.familyIndex.has_value() && data->description->data->queueFamilyProperties[ i ].queueFlags & VK_QUEUE_GRAPHICS_BIT )
                    data->queuesSet.graphic = { i, 1.f };
                else if ( !data->queuesSet.transfer.familyIndex.has_value() && data->description->data->queueFamilyProperties[ i ].queueFlags & VK_QUEUE_TRANSFER_BIT )
                    data->queuesSet.transfer = { i, 1.f };
                // else if ( !data->queuesSet.compute.familyIndex.has_value() && data->description->data->queueFamilyProperties[ i ].queueFlags & VK_QUEUE_COMPUTE_BIT )
                //     data->queuesSet.compute = i;
                else
                    break;
            }
        }
        else
        {
            if ( data->description->data->queueFamilyProperties[ 0 ].queueFlags & ( VK_QUEUE_TRANSFER_BIT | VK_QUEUE_GRAPHICS_BIT ) )
            {
                data->queuesSet = { { 0, 0, 1.f }, { 0, 0, 1.f } };
            }
        }
        data->create( DeviceCreateInfo );
    }

    types::swapchain device::bindWindow( window::types::window window )
    {
        return new swapchain { this, window };
    }

    types::swapchain device::DATA_TYPE::regSwapchain( types::swapchain swapchain )
    {
        swapchain->setup();
        return swapchain;
    }

    void device::DATA_TYPE::create( VkDeviceCreateInfo createInfo )
    {
        std::vector<VkDeviceQueueCreateInfo> QueuesCreateInfo;
        QueuesCreateInfo.reserve( queuesSet.getUniqueIndecies().size() );
        auto d { queuesSet.getUniqueIndecies() };
        for ( const auto &index : queuesSet.getUniqueIndecies() )
        {
            QueuesCreateInfo.push_back( { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO } );
            QueuesCreateInfo.back().flags            = index.second.flags;
            QueuesCreateInfo.back().queueFamilyIndex = index.first;
            QueuesCreateInfo.back().queueCount       = index.second.prioreties.size();
            QueuesCreateInfo.back().pQueuePriorities = index.second.prioreties.data();
        }
        std::vector<const char *> exts { createInfo.ppEnabledExtensionNames, createInfo.ppEnabledExtensionNames + createInfo.enabledExtensionCount };
        setExtensions( exts );
        assert( supportExtensions() );
        createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.enabledExtensionCount   = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.queueCreateInfoCount    = QueuesCreateInfo.size();
        createInfo.pQueueCreateInfos       = QueuesCreateInfo.data();
        CHECK_RESULT( vkCreateDevice( description->data->phDevice, &createInfo, ALLOCATION_CALLBACK, &handle ) );
        queuesSet.init( handle );
    }

    VkFormat device::DATA_TYPE::formatPriority( const std::vector<VkFormat> &formats, VkImageTiling ImageTiling, VkFormatFeatureFlags FormatFeatureFlags )
    {
        for ( auto format : formats )
        {
            VkFormatProperties FormatProperties;
            vkGetPhysicalDeviceFormatProperties( description->data->phDevice, format, &FormatProperties );
            switch ( ImageTiling )
            {
                case VK_IMAGE_TILING_LINEAR:
                    if ( ( FormatProperties.linearTilingFeatures & FormatFeatureFlags ) == FormatFeatureFlags )
                        return format;
                    break;
                case VK_IMAGE_TILING_OPTIMAL:
                    if ( ( FormatProperties.optimalTilingFeatures & FormatFeatureFlags ) == FormatFeatureFlags )
                        return format;
                    break;
                default:
                    break;
            }
        }
        return VK_FORMAT_UNDEFINED;
    }

    uint32_t device::DATA_TYPE::requeredMemoryTypeIndex( uint32_t type, VkMemoryPropertyFlags properties )
    {
        for ( uint32_t i { 0 }; i < description->data->memProperties.memoryTypeCount; i++ )
        {
            if ( ( type & ( 1 << i ) ) && ( ( description->data->memProperties.memoryTypes[ i ].propertyFlags & properties ) == properties ) ) return i;
        }
        SPDLOG_CRITICAL( "Failed to find suitable memory type, type: {}, properties: {}.", type, properties );
        assert( 0 );
        return -1;
    }
    // device::DATA_TYPE::vector<_T>::vector( device::DATA_TYPE *parent ) :
    //     parent { parent }
    // {
    // }

    // uint32_t device::DATA_TYPE::vector<image>::append( image *img, VkMemoryPropertyFlags properties )
    // {
    //     // capacity?
    //     VkMemoryRequirements mReq {};
    //     vkGetImageMemoryRequirements( parent->handle, img->data->ImageViewInfo.image, &mReq );
    //     VkMemoryAllocateInfo allocateInfo {};
    //     allocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    //     allocateInfo.memoryTypeIndex = parent->requeredMemoryTypeIndex( mReq.memoryTypeBits, properties );
    //     offsets[ img ]               = ( size + size % mReq.alignment );
    //     size += offsets[ img ] + mReq.size - not aviable;
    //     //  for ( auto &i : parent->images[ allocateInfo.memoryTypeIndex ].offsets )
    //     //  {
    //     //  }
    //     return allocateInfo.memoryTypeIndex;
    // }

    // types::descriptorPool device::CreatePool( void *userData )
    // {
    //     data->descriptorPools.emplace_back( std::unique_ptr<descriptorPool> { new descriptorPool { this, userData } } );
    //     return data->descriptorPools.back().get();
    // }

    // types::shader device::CreateShader( std::string path, std::string main, ShaderStage stage )
    // {
    //     return CreateShader( path.data(), main.data(), stage );
    // }

    // types::shader device::CreateShader( const char *path, const char *main, ShaderStage stage )
    // {
    //     data->shaders.emplace_back( std::unique_ptr<shader> { new shader { this, path, main, stage } } );
    //     return data->shaders.back().get();
    // }

    // types::layout device::CreateLayout( types::descriptorPool pool, void *userData )
    // {
    //     data->layouts.emplace_back( std::unique_ptr<layout> { new layout { this, pool, userData } } );
    //     return data->layouts.back().get();
    // }

    // types::pipeline device::CreatePipeline( types::layout layout, std::vector<types::shader> shaders, types::pass pass )
    // {
    //     data->pipelines.emplace_back( std::unique_ptr<pipeline> { new pipeline { this, layout, shaders, pass } } );
    //     return data->pipelines.back().get();
    // }

    const std::vector<types::DeviceDescription> &
        instance::getDevices()
    {
        if ( data->deviceDescriptions.empty() )
        {
            uint32_t _c;
            std::vector<VkPhysicalDevice> devices;
            CHECK_RESULT( vkEnumeratePhysicalDevices( data->handle, &_c, nullptr ) );
            devices.resize( _c );

            data->deviceDescriptions.resize( _c );
            CHECK_RESULT( vkEnumeratePhysicalDevices( data->handle, &_c, devices.data() ) );
            for ( uint32_t c { 0 }; c < devices.size(); ++c )
                data->deviceDescriptions[ c ] = new DeviceDescription { this, devices[ c ] };
        }
        return data->deviceDescriptions;
    }
} // namespace Engine
