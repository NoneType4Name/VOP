#include <device.hxx>
#include <EHI.hxx>
#include <RHI.hxx>

namespace Engine
{
    DeviceDescription::DeviceDescription() {
        DEFINE_DATA_FIELD };

    DeviceDescription::~DeviceDescription() {};

    void DeviceDescription::DATA_TYPE::init( VkPhysicalDevice device )
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

    device::DATA_TYPE::DATA_TYPE( DeviceDescription *description ) :
        description { description }, queuesSet { description } {}

    void device::DATA_TYPE::setupNextChain( const void *&pNext, std::vector<void *> &dataPointers, void *userPoiner )
    {
        dataPointers.resize( 1 );
        dataPointers[ 0 ] = static_cast<void *>( new VkPhysicalDeviceDescriptorIndexingFeatures {} );
        VkPhysicalDeviceDescriptorIndexingFeatures *features { static_cast<VkPhysicalDeviceDescriptorIndexingFeatures *>( dataPointers[ 0 ] ) };
        pNext                                               = dataPointers[ 0 ];
        features->sType                                     = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
        features->shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
        features->runtimeDescriptorArray                    = VK_TRUE;
        features->descriptorBindingVariableDescriptorCount  = VK_TRUE;
    }

    void device::DATA_TYPE::setupExtensions( std::vector<const char *> &deviceExtensions, void *userPoiner ) {}
    void device::DATA_TYPE::setupFeatures( VkPhysicalDeviceFeatures &features, void *userPoiner )
    {
        features.samplerAnisotropy = VK_TRUE;
        features.sampleRateShading = VK_TRUE;
    }
    void device::DATA_TYPE::setupQueueSet( queueSet &queues, VkSurfaceKHR surface, void *userPoiner )
    {
        if ( description->data->queueFamilyProperties.size() - 1 )
        {
            for ( uint32_t i { 0 }; i < description->data->queueFamilyProperties.size(); i++ )
            {
                VkBool32 presentSupport { false };
                vkGetPhysicalDeviceSurfaceSupportKHR( description->data->phDevice, i, surface, &presentSupport );
                if ( !queuesSet.graphic.familyIndex.has_value() && description->data->queueFamilyProperties[ i ].queueFlags & VK_QUEUE_GRAPHICS_BIT )
                    queuesSet.graphic = { i, 1.f };
                if ( !queuesSet.present.familyIndex.has_value() && presentSupport )
                    queuesSet.present = { i, 1.f };
                else if ( !queuesSet.transfer.familyIndex.has_value() && description->data->queueFamilyProperties[ i ].queueFlags & VK_QUEUE_TRANSFER_BIT )
                    queuesSet.transfer = { i, 1.f };
                // else if ( !queuesSet.compute.familyIndex.has_value() && description.data->queueFamilyProperties[ i ].queueFlags & VK_QUEUE_COMPUTE_BIT )
                //     queuesSet.compute = i;
                else
                    break;
            }
        }
        else
        {
            VkBool32 presentSupport { false };
            vkGetPhysicalDeviceSurfaceSupportKHR( description->data->phDevice, 0, surface, &presentSupport );
            if ( description->data->queueFamilyProperties[ 0 ].queueFlags & ( VK_QUEUE_TRANSFER_BIT | VK_QUEUE_GRAPHICS_BIT ) && presentSupport )
            {
                queuesSet = { { 0, 0, 1.f }, { 0, 0, 1.f }, { 0, 0, 1.f } };
            }
        }
    }

    device::device() = default;
    device::device( types::DeviceDescription description, window::types::window window )
    {
        auto &_data = const_cast<std ::unique_ptr<DATA_TYPE> &>( data );
        _data.reset( new DATA_TYPE { description } );

        this->data->window = window;
        data->description  = description;
        data->setupQueueSet( data->queuesSet, this->data->window->data->surface, data->window->data->instance->data->userPointer );
        std::vector<VkDeviceQueueCreateInfo> QueuesCreateInfo;
        QueuesCreateInfo.reserve( data->queuesSet.getUniqueIndecies().size() );
        // std::vector<float> _priorities( data->queuesSet.getUniqueIndecies().size() );
        std::vector<std::vector<void *>> userNexChains;
        userNexChains.reserve( data->queuesSet.getUniqueIndecies().size() );
        auto d { data->queuesSet.getUniqueIndecies() };
        for ( const auto &index : data->queuesSet.getUniqueIndecies() )
        {
            userNexChains.push_back( {} );
            QueuesCreateInfo.push_back( { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO } );
            data->queuesSet.setupNextChain( QueuesCreateInfo.back().pNext, userNexChains.back() );
            data->queuesSet.setupFlags( QueuesCreateInfo.back().flags );
            QueuesCreateInfo.back().queueFamilyIndex = index.first;
            QueuesCreateInfo.back().queueCount       = index.second.second.size();
            QueuesCreateInfo.back().pQueuePriorities = index.second.second.data();
        }
        std::vector<const char *> extensions {};
        VkPhysicalDeviceFeatures2 physicalDeviceFeatures {};
        VkDeviceCreateInfo createInfo {};
        VkPhysicalDeviceFeatures features {};
        data->setupFeatures( features, data->window->data->instance->data->userPointer );
        data->setupExtensions( extensions, data->window->data->instance->data->userPointer );
        data->setExtensions( extensions );
        assert( data->supportExtensions() );

        std::vector<void *> nextChainData;
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        data->setupNextChain( createInfo.pNext, nextChainData, data->window->data->instance->data->userPointer );
        createInfo.queueCreateInfoCount    = static_cast<uint32_t>( QueuesCreateInfo.size() );
        createInfo.pQueueCreateInfos       = QueuesCreateInfo.data();
        createInfo.pEnabledFeatures        = &features;
        createInfo.enabledExtensionCount   = static_cast<uint32_t>( data->extensions.size() );
        createInfo.ppEnabledExtensionNames = data->extensions.size() ? data->extensions.data() : nullptr;
        CHECK_RESULT( vkCreateDevice( data->description->data->phDevice, &createInfo, ALLOCATION_CALLBACK, &data->device ) );
        data->queuesSet.init( data->device );
    }

    device::~device()
    {
        vkDestroyDevice( data->device, ALLOCATION_CALLBACK );
    }

    const std::vector<types::DeviceDescription> instance::GetDevices()
    {
        if ( data->deviceDescriptions.empty() )
        {
            uint32_t _c;
            std::vector<VkPhysicalDevice> devices;
            CHECK_RESULT( vkEnumeratePhysicalDevices( data->handle, &_c, nullptr ) );
            devices.resize( _c );

            data->deviceDescriptions.resize( _c );
            CHECK_RESULT( vkEnumeratePhysicalDevices( data->handle, &_c, devices.data() ) );
            for ( uint32_t c { 0 }; c < devices.size(); c++ )
            {
                data->deviceDescriptions[ c ].reset( new DeviceDescription );
                data->deviceDescriptions[ c ]->data->init( devices[ c ] );
                data->deviceDescriptions[ c ]->name  = data->deviceDescriptions[ c ]->data->properties.deviceName;
                data->deviceDescriptions[ c ]->type  = tools::VkDevTypeToEngineDevType( data->deviceDescriptions[ c ]->data->properties.deviceType );
                data->deviceDescriptions[ c ]->grade = data->deviceDescriptions[ c ]->data->queueFamilyProperties.size() + data->deviceDescriptions[ c ]->type;
            }
        }
        std::vector<types::DeviceDescription> ret;
        ret.reserve( data->deviceDescriptions.size() );
        for ( auto &dscrpt : data->deviceDescriptions )
            ret.push_back( dscrpt.get() );
        return ret;
    }

    namespace tools
    {
        uint32_t requeredMemoryTypeIndex( types::device device, uint32_t type, VkMemoryPropertyFlags properties )
        {
            for ( uint32_t i { 0 }; i < device->data->description->data->memProperties.memoryTypeCount; i++ )
            {
                if ( ( type & ( 1 << i ) ) && ( ( device->data->description->data->memProperties.memoryTypes[ i ].propertyFlags & properties ) == properties ) ) return i;
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
