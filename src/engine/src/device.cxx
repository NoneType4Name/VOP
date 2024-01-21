#include <device.hxx>
#include <surface.hxx>
#include <swapchain.hxx>
#include <instance.hxx>

namespace Engine
{
    deviceDescription::deviceDescription( instance *instance, VkPhysicalDevice phDevice )
    {
        DEFINE_DATA_FIELD( instance, phDevice );
    }

    deviceDescription::~deviceDescription() {};

    device::DATA_TYPE::DATA_TYPE( types::device parent, types::deviceDescription description ) :
        parent { parent }, description { description }, queuesSet { parent }
    {
    }

    device::DATA_TYPE::~DATA_TYPE()
    {
    }

    device::device( types::deviceDescription description, std::vector<window::types::window> windows ) :
        device( 1, description, windows )
    {
        VkDeviceCreateInfo DeviceCreateInfo {};
        // VkPhysicalDeviceDescriptorIndexingFeatures indexFeatures {};
        // DeviceCreateInfo.pNext                                  = &indexFeatures;
        // indexFeatures.sType                                     = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
        // indexFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
        // indexFeatures.runtimeDescriptorArray                    = VK_TRUE;
        // indexFeatures.descriptorBindingVariableDescriptorCount  = VK_TRUE;
        VkPhysicalDeviceFeatures features {};
        features.samplerAnisotropy = VK_TRUE;
        features.sampleRateShading = VK_TRUE;
        if ( data->description->data->queueFamilyProperties.size() - 1 )
        {
            for ( uint32_t i { 0 }; i < data->description->data->queueFamilyProperties.size(); i++ )
            {
                VkBool32 presentSupport { false };
                for ( auto &wnd : windows )
                {
                    vkGetPhysicalDeviceSurfaceSupportKHR( data->description->data->phDevice, i, wnd->surface, &presentSupport );
                    if ( !presentSupport )
                        break;
                }

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
            for ( auto &wnd : windows )
            {
                vkGetPhysicalDeviceSurfaceSupportKHR( data->description->data->phDevice, 0, wnd->surface, &presentSupport );
            }
            if ( data->description->data->queueFamilyProperties[ 0 ].queueFlags & ( VK_QUEUE_TRANSFER_BIT | VK_QUEUE_GRAPHICS_BIT ) && presentSupport )
            {
                data->queuesSet = { { 0, 0, 1.f }, { 0, 0, 1.f }, { 0, 0, 1.f } };
            }
        }
        VkPhysicalDeviceFeatures2 physicalDeviceFeatures {};
        DeviceCreateInfo.pEnabledFeatures = &features;
        data->create( DeviceCreateInfo );
        for ( auto &wnd : windows )
        {
            new swapchain { this, wnd };
        }

        VkCommandPoolCreateInfo poolCI {};
        poolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        if ( data->queuesSet.graphic.familyIndex.has_value() )
        {
            poolCI.queueFamilyIndex = data->queuesSet.graphic.familyIndex.value();
            CHECK_RESULT( vkCreateCommandPool( handle, &poolCI, ALLOCATION_CALLBACK, &grapchicPool ) );
        }
        if ( data->queuesSet.transfer.familyIndex.has_value() )
        {
            poolCI.queueFamilyIndex = data->queuesSet.transfer.familyIndex.value();
            CHECK_RESULT( vkCreateCommandPool( handle, &poolCI, ALLOCATION_CALLBACK, &transferPool ) );
        }
        if ( data->queuesSet.present.familyIndex.has_value() )
        {
            poolCI.queueFamilyIndex = data->queuesSet.present.familyIndex.value();
            CHECK_RESULT( vkCreateCommandPool( handle, &poolCI, ALLOCATION_CALLBACK, &presentPool ) );
        }
    }

    device::device( bool, types::deviceDescription description, std::vector<window::types::window> windows ) :
        memory { this }
    {
        DEFINE_DATA_FIELD( description );
        data->description->data->instance->data->devices.emplace( this );
    }

    device::~device()
    {
        if ( grapchicPool )
            vkDestroyCommandPool( handle, grapchicPool, ALLOCATION_CALLBACK );
        if ( transferPool )
            vkDestroyCommandPool( handle, transferPool, ALLOCATION_CALLBACK );
        if ( presentPool )
            vkDestroyCommandPool( handle, presentPool, ALLOCATION_CALLBACK );
        for ( auto &img : data->images )
            delete img;
        for ( auto &buf : data->buffers )
            delete buf;
        // data->pipelines.clear();
        // data->shaders.clear();
        // data->layouts.clear();
        // data->descriptorPools.clear();
        vkDestroyDevice( handle, ALLOCATION_CALLBACK );
        for ( auto &s : data->swapchains )
            delete s;
        data->description->data->instance->data->devices.erase( this );
    }

    deviceDescription::DATA_TYPE::DATA_TYPE( types::deviceDescription parent, struct instance *instance, VkPhysicalDevice device ) :
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

    deviceDescription::DATA_TYPE::~DATA_TYPE()
    {
    }

    types::swapchain device::getLink( window::types::window window )
    {
        for ( auto &swp : data->swapchains )
        {
            if ( swp->data->window == window )
                return swp;
        }
        return nullptr;
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
        CHECK_RESULT( vkCreateDevice( description->data->phDevice, &createInfo, ALLOCATION_CALLBACK, &parent->handle ) );
        queuesSet.init( parent->handle );
    }

    VkFormat device::formatPriority( const std::vector<VkFormat> &formats, VkImageTiling ImageTiling, VkFormatFeatureFlags FormatFeatureFlags )
    {
        for ( auto format : formats )
        {
            VkFormatProperties FormatProperties;
            vkGetPhysicalDeviceFormatProperties( data->description->data->phDevice, format, &FormatProperties );
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

    uint32_t device::requeredMemoryTypeIndex( uint32_t type, VkMemoryPropertyFlags properties )
    {
        for ( uint32_t i { 0 }; i < data->description->data->memProperties.memoryTypeCount; i++ )
        {
            if ( ( type & ( 1 << i ) ) && ( ( data->description->data->memProperties.memoryTypes[ i ].propertyFlags & properties ) == properties ) ) return i;
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

    const std::vector<types::deviceDescription> &
        instance::getDevices()
    {
        if ( data->deviceDescriptions.empty() )
        {
            uint32_t _c;
            std::vector<VkPhysicalDevice> devices;
            CHECK_RESULT( vkEnumeratePhysicalDevices( handle, &_c, nullptr ) );
            devices.resize( _c );

            data->deviceDescriptions.resize( _c );
            CHECK_RESULT( vkEnumeratePhysicalDevices( handle, &_c, devices.data() ) );
            for ( uint32_t c { 0 }; c < devices.size(); ++c )
                data->deviceDescriptions[ c ] = new deviceDescription { this, devices[ c ] };
        }
        return data->deviceDescriptions;
    }
} // namespace Engine
