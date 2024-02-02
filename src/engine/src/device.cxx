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
        parent { parent }, description { description }
    {
    }

    device::DATA_TYPE::~DATA_TYPE()
    {
    }

    device::device( types::deviceDescription description, std::vector<window::types::window> windows ) :
        device( 1, description, windows )
    {
        VkDeviceCreateInfo DeviceCreateInfo {};
        VkPhysicalDeviceFeatures features {};
        features.samplerAnisotropy = VK_TRUE;
        features.sampleRateShading = VK_TRUE;
        std::vector<VkDeviceQueueCreateInfo> queues;
        std::vector<float> priorities;
        uint32_t i { 0 };
        for ( uint32_t i { 0 }; i < data->description->data->queueFamilyProperties.size(); ++i )
        {
            VkBool32 present;
            for ( const auto &wnd : windows )
            {
                vkGetPhysicalDeviceSurfaceSupportKHR( data->description->data->phDevice, i, wnd->surface, &present );
                if ( !present )
                    break;
            }
            if ( data->description->data->queueFamilyProperties[ i ].queueFlags & ( VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT ) && present )
            {
                priorities.emplace_back( 1.f );
                queues.emplace_back( VkDeviceQueueCreateInfo { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, nullptr, 0, i, 1, priorities.data() } );
                break;
            }
        }
        DeviceCreateInfo.queueCreateInfoCount = queues.size();
        DeviceCreateInfo.pQueueCreateInfos    = queues.data();
        VkPhysicalDeviceFeatures2 physicalDeviceFeatures {};
        DeviceCreateInfo.pEnabledFeatures = &features;
        data->create( DeviceCreateInfo );
        universalQueue = getQueue( 0, 0 );
        for ( auto &wnd : windows )
        {
            new swapchain { this, wnd };
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
        auto img { data->images.begin() };
        while ( img != data->images.end() )
            delete *img++;
        auto buf { data->buffers.begin() };
        while ( buf != data->buffers.end() )
            delete *buf++;
        auto rdp { data->renderpasses.begin() };
        while ( rdp != data->renderpasses.end() )
            delete *rdp++;
        auto que { data->queues.begin() };
        while ( que != data->queues.end() )
            delete *que++;
        // data->pipelines.clear();
        // data->shaders.clear();
        // data->layouts.clear();
        // data->descriptorPools.clear();
        vkDestroyDevice( handle, ALLOCATION_CALLBACK );
        auto swp { data->swapchains.begin() };
        while ( swp != data->swapchains.end() )
            delete *swp++;
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

    void device::DATA_TYPE::create( VkDeviceCreateInfo createInfo )
    {
        std::vector<const char *> exts { createInfo.ppEnabledExtensionNames, createInfo.ppEnabledExtensionNames + createInfo.enabledExtensionCount };
        setExtensions( exts );
        assert( supportExtensions() );
        createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.enabledExtensionCount   = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();
        CHECK_RESULT( vkCreateDevice( description->data->phDevice, &createInfo, ALLOCATION_CALLBACK, &parent->handle ) );
        _queues.resize( createInfo.queueCreateInfoCount );
        uint32_t i { 0 };
        for ( uint32_t i { 0 }; i < createInfo.queueCreateInfoCount; ++i )
        {
            _queues[ createInfo.pQueueCreateInfos[ i ].queueFamilyIndex ].resize( createInfo.pQueueCreateInfos[ i ].queueCount );
            for ( uint32_t ii { 0 }; ii < createInfo.pQueueCreateInfos[ i ].queueCount; ++ii )
            {
                _queues[ createInfo.pQueueCreateInfos[ i ].queueFamilyIndex ][ ii ].first  = createInfo.pQueueCreateInfos[ i ].pQueuePriorities[ ii ];
                _queues[ createInfo.pQueueCreateInfos[ i ].queueFamilyIndex ][ ii ].second = new queue { parent, createInfo.pQueueCreateInfos[ i ].queueFamilyIndex, ii, createInfo.pQueueCreateInfos[ i ].pQueuePriorities[ ii ] };
            }
        }
    }

    types::swapchain device::getLink( window::types::window window ) const noexcept
    {
        for ( auto &swp : data->swapchains )
        {
            if ( swp->data->window == window )
                return swp;
        }
        return nullptr;
    }

    types::queue device::getQueue( uint32_t familyIndex, uint32_t index ) const noexcept
    {
        return data->_queues[ familyIndex ][ index ].second;
    }

    VkFormat device::formatPriority( const std::vector<VkFormat> &formats, VkImageTiling ImageTiling, VkFormatFeatureFlags FormatFeatureFlags ) const noexcept
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

    uint32_t device::requeredMemoryTypeIndex( uint32_t type, VkMemoryPropertyFlags properties ) const
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

    const std::vector<types::deviceDescription> &instance::getDevices()
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
