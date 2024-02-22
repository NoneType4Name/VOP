#include <device.hxx>
#include <surface.hxx>
#include <swapchain.hxx>
#include <instance.hxx>

namespace Engine
{
    deviceDescription::deviceDescription( instance *instance, VkPhysicalDevice phDevice )
    {
        OBJECTIVE_VULKAN_OBJECTIVE_VULKAN_DEFINE_DATA_FIELD( instance, phDevice );
    }

    deviceDescription::~deviceDescription() {};

    device::OBJECTIVE_VULKAN_DATA_TYPE::OBJECTIVE_VULKAN_DATA_TYPE( types::device parent, types::deviceDescription description ) :
        parent { parent }, description { description }
    {
    }

    device::OBJECTIVE_VULKAN_DATA_TYPE::~OBJECTIVE_VULKAN_DATA_TYPE()
    {
    }

    device::device( types::deviceDescription description, std::vector<types::surface> windows ) :
        memory { new _memory { this } }
    {
        OBJECTIVE_VULKAN_OBJECTIVE_VULKAN_DEFINE_DATA_FIELD( description );
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
        for ( const auto &shr : data->shaders )
            vkDestroyShaderModule( handle, shr.second, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
        auto que { data->queues.begin() };
        while ( que != data->queues.end() )
            delete *que++;
        delete memory;
        vkDestroyDevice( handle, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
        auto swp { data->swapchains.begin() };
        while ( swp != data->swapchains.end() )
            delete *swp++;
        data->description->data->instance->data->devices.erase( this );
    }

    deviceDescription::OBJECTIVE_VULKAN_DATA_TYPE::OBJECTIVE_VULKAN_DATA_TYPE( types::deviceDescription parent, struct instance *instance, VkPhysicalDevice device ) :
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

    deviceDescription::OBJECTIVE_VULKAN_DATA_TYPE::~OBJECTIVE_VULKAN_DATA_TYPE()
    {
    }

    void device::OBJECTIVE_VULKAN_DATA_TYPE::create( VkDeviceCreateInfo createInfo )
    {
        std::vector<const char *> exts { createInfo.ppEnabledExtensionNames, createInfo.ppEnabledExtensionNames + createInfo.enabledExtensionCount };
        setExtensions( exts );
        assert( supportExtensions() );
        createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.enabledExtensionCount   = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();
        OBJECTIVE_VULKAN_CHECK_RESULT( vkCreateDevice( description->data->phDevice, &createInfo, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &parent->handle ) );
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

    types::swapchain device::getLink( types::surface surface ) const noexcept
    {
        for ( auto &swp : data->swapchains )
        {
            if ( swp->data->surface == surface )
                return swp;
        }
        return nullptr;
    }

    types::queue device::getQueue( uint32_t familyIndex, uint32_t index ) const noexcept
    {
        return data->_queues[ familyIndex ][ index ].second;
    }

    VkShaderModule device::loadShader( const char *path, VkShaderModuleCreateFlags flags, const void *pNext )
    {
        if ( data->shaders.contains( path ) )
            return data->shaders[ path ];
        std::ifstream File { path, std::fstream::ate | std::fstream::binary };
        if ( !File.is_open() )
        {
            SPDLOG_CRITICAL( "Failed to open {}.", path );
            return nullptr;
        }
        auto shBsize { File.tellg() };
        std::vector<char> Data( shBsize );
        File.seekg( 0 );
        File.read( Data.data(), shBsize );
        File.close();
        VkShaderModuleCreateInfo createInfo {};
        createInfo.sType      = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.flags      = flags;
        createInfo.pNext      = pNext;
        createInfo.codeSize   = Data.size();
        createInfo.pCode      = reinterpret_cast<const uint32_t *>( Data.data() );
        data->shaders[ path ] = nullptr;
        OBJECTIVE_VULKAN_CHECK_RESULT( vkCreateShaderModule( handle, &createInfo, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &data->shaders[ path ] ) );
        return data->shaders[ path ];
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

    const std::vector<types::deviceDescription> &instance::getDevices()
    {
        if ( data->deviceDescriptions.empty() )
        {
            uint32_t _c;
            std::vector<VkPhysicalDevice> devices;
            OBJECTIVE_VULKAN_CHECK_RESULT( vkEnumeratePhysicalDevices( handle, &_c, nullptr ) );
            devices.resize( _c );

            data->deviceDescriptions.resize( _c );
            OBJECTIVE_VULKAN_CHECK_RESULT( vkEnumeratePhysicalDevices( handle, &_c, devices.data() ) );
            for ( uint32_t c { 0 }; c < devices.size(); ++c )
                data->deviceDescriptions[ c ] = new deviceDescription { this, devices[ c ] };
        }
        return data->deviceDescriptions;
    }
} // namespace Engine
