#include <device.hxx>
#include <queue.hxx>
#include <RHI.hxx>

namespace Engine
{
    namespace tools
    {
        namespace
        {
            std::vector<VkExtensionProperties> _extensions;
            VkPhysicalDevice _phDevice{ nullptr };
            VkDevice _device{ nullptr };
            Queues _queues;
            std::vector<VkPhysicalDevice> _allDevices{};
        } // namespace

        const std::vector<VkPhysicalDevice> getDevices()
        {
            if( _allDevices.empty() )
            {
                uint32_t _c{ 0 };
                vkEnumeratePhysicalDevices( getInstance(), &_c, nullptr );
                _allDevices.resize( _c );
                vkEnumeratePhysicalDevices( getInstance(), &_c, _allDevices.data() );
            }
            return _allDevices;
        }

        void getSuitableDevices( std::vector<PhysicalDevice> &devices )
        {
            getSuitableDevices( devices, 0 );
        }

        void getSuitableDevices( std::vector<PhysicalDevice> &devices, uint8_t types )
        {
            auto _all_devices{ tools::getDevices() };
            devices.reserve( _all_devices.size() );
            devices.resize( 0 );
            if( types )
            {
                for( auto dev : _all_devices )
                {
                    VkPhysicalDeviceMemoryProperties Mproperties{};
                    VkPhysicalDeviceProperties properties{};
                    VkPhysicalDeviceFeatures features{};
                    vkGetPhysicalDeviceMemoryProperties( dev, &Mproperties );
                    vkGetPhysicalDeviceProperties( dev, &properties );
                    vkGetPhysicalDeviceFeatures( dev, &features );
                    if( ( ( static_cast<VkPhysicalDeviceType>( types ) & properties.deviceType ) == properties.deviceType ) )
                    {
                        devices.push_back( { properties.deviceName, dev } );
                    }
                }
            }
        }

        void getSuitableDevices( std::vector<Device> &devices )
        {
            getSuitableDevices( devices, 0 );
        }

        void getSuitableDevices( std::vector<Device> &devices, uint8_t types )
        {
            auto _all_devices{ tools::getDevices() };
            devices.reserve( _all_devices.size() );
            devices.resize( 0 );
            if( types )
            {
                for( auto dev : _all_devices )
                {
                    VkPhysicalDeviceMemoryProperties Mproperties{};
                    VkPhysicalDeviceProperties properties{};
                    VkPhysicalDeviceFeatures features{};
                    vkGetPhysicalDeviceMemoryProperties( dev, &Mproperties );
                    vkGetPhysicalDeviceProperties( dev, &properties );
                    vkGetPhysicalDeviceFeatures( dev, &features );
                    if( ( ( static_cast<VkPhysicalDeviceType>( types ) & properties.deviceType ) == properties.deviceType ) )
                    {
                        devices.push_back( { properties.deviceName, dev } );
                    }
                }
            }
        }

        bool deviceSupportExtensionsAndLayers( std::vector<const char *> extensions, std::vector<const char *> layers )
        {
            uint32_t _c{ 0 };
            vkEnumerateDeviceExtensionProperties( _phDevice, nullptr, &_c, nullptr );
            std::vector<VkExtensionProperties> AvilableExtNames{ _c };
            vkEnumerateDeviceExtensionProperties( _phDevice, nullptr, &_c, AvilableExtNames.data() );
            std::set<std::string> tmpRequeredDeviceExts{ extensions.begin(), extensions.end() };
            for( const auto &ext : AvilableExtNames )
            {
                tmpRequeredDeviceExts.erase( ext.extensionName );
            }

            vkEnumerateDeviceLayerProperties( _phDevice, &_c, nullptr );
            std::vector<VkLayerProperties> AvilableLNames{ _c };
            vkEnumerateDeviceLayerProperties( _phDevice, &_c, AvilableLNames.data() );
            std::set<std::string> tmpRequeredDeviceL{ layers.begin(), layers.end() };
            for( const auto &l : AvilableLNames )
            {
                tmpRequeredDeviceL.erase( l.layerName );
            }

            if( tmpRequeredDeviceExts.size() )
            {
                std::string e{ "Not avilable device extensions:\n" };
                for( const auto &ext : tmpRequeredDeviceExts )
                {
                    auto s{ std::format( "\t{}\n", ext ) };
                }
                SPDLOG_CRITICAL( e );
            }
            if( tmpRequeredDeviceL.size() )
            {
                std::string e{ "Not avilable device extensions:\n" };
                for( const auto &l : tmpRequeredDeviceL )
                {
                    auto s{ std::format( "\t{}\n", l ) };
                }
                SPDLOG_CRITICAL( e );
            }

            return tmpRequeredDeviceExts.empty() && tmpRequeredDeviceL.empty();
        }

        void createDevice( VkPhysicalDevice phDevice )
        {
            _phDevice = phDevice;
            uint32_t _c{ 0 };
            vkGetPhysicalDeviceQueueFamilyProperties( _phDevice, &_c, nullptr );
            std::vector<VkQueueFamilyProperties> queueFamilyProperties{ _c };
            vkGetPhysicalDeviceQueueFamilyProperties( _phDevice, &_c, queueFamilyProperties.data() );
            std::vector<VkDeviceQueueCreateInfo> QueuesCreateInfo( _queues.count() );
            std::vector<float> _priorities( _queues.count() );
            float *_pPriorities{ _priorities.data() };
            _queues = getIndecies( _phDevice );
            for( size_t i{ 0 }; i < _queues.count(); i++ )
            {
                QueuesCreateInfo[ i ] = queueCreateInfo( _queues[ i ].GetQueueIndex(), 1, _pPriorities );
                *_pPriorities++       = 1.f;
            }

            VkPhysicalDeviceFeatures enabledFeatures{};
            enabledFeatures.samplerAnisotropy = VK_TRUE;
            enabledFeatures.sampleRateShading = VK_TRUE;

            VkPhysicalDeviceDescriptorIndexingFeaturesEXT PhysicalDeviceDescriptorIndexingFeatures{};
            PhysicalDeviceDescriptorIndexingFeatures.sType                                     = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
            PhysicalDeviceDescriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
            PhysicalDeviceDescriptorIndexingFeatures.runtimeDescriptorArray                    = VK_TRUE;
            PhysicalDeviceDescriptorIndexingFeatures.descriptorBindingVariableDescriptorCount  = VK_TRUE;

            std::vector<const char *> Layers, Extensions{};
            getDeviceLayersAndExtension( Layers, Extensions );
            assert( deviceSupportExtensionsAndLayers );
            VkDeviceCreateInfo createInfo{};
            createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            createInfo.pNext                   = &PhysicalDeviceDescriptorIndexingFeatures;
            createInfo.queueCreateInfoCount    = static_cast<uint32_t>( QueuesCreateInfo.size() );
            createInfo.pQueueCreateInfos       = QueuesCreateInfo.data();
            createInfo.pEnabledFeatures        = &enabledFeatures;
            createInfo.enabledExtensionCount   = static_cast<uint32_t>( Extensions.size() );
            createInfo.ppEnabledExtensionNames = Extensions.size() ? Extensions.data() : nullptr;
            createInfo.enabledLayerCount       = static_cast<uint32_t>( Layers.size() );
            createInfo.ppEnabledLayerNames     = Layers.size() ? Layers.data() : nullptr;
            CHECK_RESULT( vkCreateDevice( _phDevice, &createInfo, nullptr, &_device ) );
        }

        void destroyDevice()
        {
            vkDestroyDevice( _device, nullptr );
        }
        const VkDevice getDevice()
        {
            return _device;
        }

        const VkPhysicalDevice getPhysicalDevice()
        {
            return _phDevice;
        }
    } // namespace tools
    PhysicalDevice::PhysicalDevice() = default;
    PhysicalDevice::PhysicalDevice( const char *name, VkPhysicalDevice handle )
    {
        this->name = name;
        _handle    = handle;
    }

    const VkPhysicalDevice PhysicalDevice::getHandle() const
    {
        return _handle;
    }

    void PhysicalDevice::setHandle( VkPhysicalDevice device )
    {
        _handle = device;
    }

    void PhysicalDevice::setName( const char *name )
    {
        this->name = name;
    }
} // namespace Engine
