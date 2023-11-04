#include <device.hxx>
#include <EHI.hxx>
#include <RHI.hxx>

namespace Engine
{
    instance::data::~data()
    {
        deviceDescriptions.clear();
    }

    void device::data::setupQueues( tools::queueSet &queues )
    {
        if ( description->data->queueFamilyProperties.size() - 1 )
        {
            for ( uint32_t i { 0 }; i < description->data->queueFamilyProperties.size(); i++ )
            {
                VkBool32 presentSupport { false };
                vkGetPhysicalDeviceSurfaceSupportKHR( description->data->phDevice, i, window->data->surface, &presentSupport );
                if ( !queuesSet.graphic.inited() && description->data->queueFamilyProperties[ i ].queueFlags & VK_QUEUE_GRAPHICS_BIT )
                {
                    queuesSet.graphic = i;
                }
                if ( !queuesSet.present.inited() && presentSupport )
                    queuesSet.present = i;
                else if ( !queuesSet.transfer.inited() && description->data->queueFamilyProperties[ i ].queueFlags & VK_QUEUE_TRANSFER_BIT )
                    queuesSet.transfer = i;
                // else if ( !queuesSet.compute.has_value() && description.data->queueFamilyProperties[ i ].queueFlags & VK_QUEUE_COMPUTE_BIT )
                //     queuesSet.compute = i;
                else
                    break;
            }
        }
        else
        {
            VkBool32 presentSupport { false };
            vkGetPhysicalDeviceSurfaceSupportKHR( description->data->phDevice, 0, window->data->surface, &presentSupport );
            if ( description->data->queueFamilyProperties[ 0 ].queueFlags & ( VK_QUEUE_TRANSFER_BIT | VK_QUEUE_GRAPHICS_BIT ) && presentSupport )
            {
                queuesSet = { 0, 0, 0 };
            }
        }
    }

    void DeviceDescription::data::init( VkPhysicalDevice device )
    {
        phDevice = device;
        uint32_t c;
        vkGetPhysicalDeviceQueueFamilyProperties( device, &c, nullptr );
        queueFamilyProperties.resize( c );
        vkGetPhysicalDeviceQueueFamilyProperties( device, &c, queueFamilyProperties.data() );
        vkGetPhysicalDeviceMemoryProperties( device, &memProperties );
        vkGetPhysicalDeviceProperties( device, &properties );
        vkGetPhysicalDeviceFeatures( device, &features );
    }

    void device::data::setupNextChain( const void *&nextChainData )
    {
        VkPhysicalDeviceDescriptorIndexingFeatures features {};
        features.sType                                     = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
        features.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
        features.runtimeDescriptorArray                    = VK_TRUE;
        features.descriptorBindingVariableDescriptorCount  = VK_TRUE;
        nextChainData                                      = &features;
    }

    device::device( types::DeviceDescription description )
    {
        data->description = description;
        data->setupQueues( data->queuesSet );
        std::vector<VkDeviceQueueCreateInfo> QueuesCreateInfo;
        QueuesCreateInfo.reserve( data->queuesSet.getUniqueIndecies().size() );
        // std::vector<float> _priorities( data->queuesSet.getUniqueIndecies().size() );
        for ( const auto &index : data->queuesSet.getUniqueIndecies() )
        {
            QueuesCreateInfo.push_back( { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO } );
            data->queuesSet.setupNextChain( QueuesCreateInfo.back().pNext );
            data->queuesSet.setupFlags( QueuesCreateInfo.back().flags );
            QueuesCreateInfo.back().queueFamilyIndex = index.first;
            QueuesCreateInfo.back().queueCount       = index.second.first;
            QueuesCreateInfo.back().pQueuePriorities = index.second.second.data();
        }
        std::vector<const char *> extensions {};
        VkPhysicalDeviceFeatures2 physicalDeviceFeatures {};
        VkDeviceCreateInfo createInfo {};
        data->setupFeatures( data->description->data->features );
        data->setupExtensions( extensions );
        data->setExtensions( extensions );
        assert( data->supportExtensions() );

        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        data->setupNextChain( createInfo.pNext );
        createInfo.queueCreateInfoCount    = static_cast<uint32_t>( QueuesCreateInfo.size() );
        createInfo.pQueueCreateInfos       = QueuesCreateInfo.data();
        createInfo.pEnabledFeatures        = &data->description->data->features;
        createInfo.enabledExtensionCount   = static_cast<uint32_t>( extensions.size() );
        createInfo.ppEnabledExtensionNames = extensions.size() ? extensions.data() : nullptr;
        CHECK_RESULT( vkCreateDevice( data->description->data->phDevice, &createInfo, ALLOCATION_CALLBACK, &data->device ) );
        // _queues.init( _device );
    }

    device::~device()
    {
        vkDestroyDevice( data->device, ALLOCATION_CALLBACK );
    }

    const std::vector<types::DeviceDescription> &instance::GetDevices() const
    {
        if ( !data->deviceDescriptions.size() )
            return data->deviceDescriptions;
        uint32_t _c;
        std::vector<VkPhysicalDevice> devices;
        CHECK_RESULT( vkEnumeratePhysicalDevices( data->handle, &_c, nullptr ) );
        devices.resize( _c );
        data->deviceDescriptions.resize( _c );
        CHECK_RESULT( vkEnumeratePhysicalDevices( data->handle, &_c, devices.data() ) );
        for ( uint32_t c { 0 }; c < devices.size(); c++ )
        {
            data->deviceDescriptions[ c ]->data->init( devices[ c ] );
            data->deviceDescriptions[ c ]->name  = data->deviceDescriptions[ c ]->data->properties.deviceName;
            data->deviceDescriptions[ c ]->type  = tools::VkDevTypeToEngineDevType( data->deviceDescriptions[ c ]->data->properties.deviceType );
            data->deviceDescriptions[ c ]->grade = data->deviceDescriptions[ c ]->data->queueFamilyProperties.size() + data->deviceDescriptions[ c ]->type;
        }
        return data->deviceDescriptions;
    }

    namespace tools
    {

        // void getSuitableDevices( std::vector<Device> &devices )
        // {
        //     getSuitableDevices( devices, 0 );
        // }

        // void getSuitableDevices( std::vector<Device> &devices, uint8_t types )
        // {
        //     auto _all_devices { tools::getDevices() };
        //     devices.reserve( _all_devices.size() );
        //     devices.resize( 0 );
        //     if ( types )
        //     {
        //         for ( auto &dev : _all_devices )
        //         {
        //             VkPhysicalDeviceMemoryProperties Mproperties {};
        //             VkPhysicalDeviceProperties properties {};
        //             VkPhysicalDeviceFeatures features {};
        //             vkGetPhysicalDeviceMemoryProperties( dev, &Mproperties );
        //             vkGetPhysicalDeviceProperties( dev, &properties );
        //             vkGetPhysicalDeviceFeatures( dev, &features );
        //             if ( ( ( static_cast<VkPhysicalDeviceType>( types ) & properties.deviceType ) == properties.deviceType ) )
        //             {
        //                 devices.push_back( { properties.deviceName, dev } );
        //             }
        //         }
        //     }
        // }

        // void createDevice( VkPhysicalDevice phDevice )
        // {
        //     _phDevice = phDevice;
        //     uint32_t _c { 0 };
        //     vkGetPhysicalDeviceQueueFamilyProperties( _phDevice, &_c, nullptr );
        //     std::vector<VkQueueFamilyProperties> queueFamilyProperties { _c };
        //     vkGetPhysicalDeviceQueueFamilyProperties( _phDevice, &_c, queueFamilyProperties.data() );
        //     _queues = getIndecies( _phDevice );
        //     std::vector<VkDeviceQueueCreateInfo> QueuesCreateInfo( _queues.getUniqueIndeciesCount().size() );
        //     std::vector<float> _priorities( _queues.getUniqueIndeciesCount().size() );
        //     _c = 0;
        //     for ( const auto &index : _queues.getUniqueIndeciesCount() )
        //         QueuesCreateInfo[ _c++ ] = queueCreateInfo( index.first, index.second.first, index.second.second.data() );

        // VkPhysicalDeviceFeatures enabledFeatures {};
        // enabledFeatures.samplerAnisotropy = VK_TRUE;
        // enabledFeatures.sampleRateShading = VK_TRUE;

        // VkPhysicalDeviceDescriptorIndexingFeaturesEXT PhysicalDeviceDescriptorIndexingFeatures {};
        // PhysicalDeviceDescriptorIndexingFeatures.sType                                     = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
        // PhysicalDeviceDescriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
        // PhysicalDeviceDescriptorIndexingFeatures.runtimeDescriptorArray                    = VK_TRUE;
        // PhysicalDeviceDescriptorIndexingFeatures.descriptorBindingVariableDescriptorCount  = VK_TRUE;

        // VkPhysicalDeviceFeatures2 physicalDeviceFeatures {};
        // physicalDeviceFeatures.sType    = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        // physicalDeviceFeatures.pNext    = &PhysicalDeviceDescriptorIndexingFeatures;
        // physicalDeviceFeatures.features = enabledFeatures;

        //     std::vector<const char *> Extensions {};
        //     getDeviceExtensions( Extensions );
        //     assert( isDeviceSupportExtensions( Extensions ) );
        //     VkDeviceCreateInfo createInfo {};
        //     createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        //     createInfo.pNext                   = &physicalDeviceFeatures;
        //     createInfo.queueCreateInfoCount    = static_cast<uint32_t>( QueuesCreateInfo.size() );
        //     createInfo.pQueueCreateInfos       = QueuesCreateInfo.data();
        //     createInfo.pEnabledFeatures        = nullptr;
        //     createInfo.enabledExtensionCount   = static_cast<uint32_t>( Extensions.size() );
        //     createInfo.ppEnabledExtensionNames = Extensions.size() ? Extensions.data() : nullptr;
        //     CHECK_RESULT( vkCreateDevice( _phDevice, &createInfo, nullptr, &_device ) );
        //     _queues.init( _device );
        // }

        uint32_t requeredMemoryTypeIndex( device device, uint32_t type, VkMemoryPropertyFlags properties )
        {
            for ( uint32_t i { 0 }; i < device.data->description->data->memProperties.memoryTypeCount; i++ )
            {
                if ( ( type & ( 1 << i ) ) && ( ( device.data->description->data->memProperties.memoryTypes[ i ].propertyFlags & properties ) == properties ) ) return i;
            }
            SPDLOG_CRITICAL( "Failed to find suitable memory type, type: {}, properties: {}.", type, properties );
            assert( 0 );
            return -1;
        }

        inline DeviceType VkDevTypeToEngineDevType( VkPhysicalDeviceType type )
        {
            switch ( type )
            {
                case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                    return DeviceType::INTEGRATED_GPU;
                case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                    return DeviceType::DISCRETE_GPU;
                case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                    return DeviceType::VIRTUAL_GPU;
                case VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_CPU:
                    return DeviceType::CPU;
                default:
                    return DeviceType::OTHER;
            }
        }
    } // namespace tools
} // namespace Engine
