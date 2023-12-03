#include <device.hxx>
#include <shader.hxx>
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
        description { description }, queuesSet { description }
    {
    }

    device::DATA_TYPE::~DATA_TYPE()
    {
    }

    void InstanceSetup::deviceNextChain( types::device device, const void *&pNext, std::vector<void *> &dataPointers, void *userPoiner )
    {
        dataPointers.resize( 1 );
        dataPointers[ 0 ] = new VkPhysicalDeviceDescriptorIndexingFeatures {};
        VkPhysicalDeviceDescriptorIndexingFeatures *features { static_cast<VkPhysicalDeviceDescriptorIndexingFeatures *>( dataPointers[ 0 ] ) };
        pNext                                               = dataPointers[ 0 ];
        features->sType                                     = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
        features->shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
        features->runtimeDescriptorArray                    = VK_TRUE;
        features->descriptorBindingVariableDescriptorCount  = VK_TRUE;
    }
    void InstanceSetup::deviceNextChainClear( types::device device, std::vector<void *> &dataPointers, void *userPoiner )
    {
        delete static_cast<VkPhysicalDeviceDescriptorIndexingFeatures *>( dataPointers[ 0 ] );
        dataPointers.clear();
    }

    void InstanceSetup::deviceExtensions( types::device device, std::vector<const char *> &deviceExtensions, void *userPoiner ) {}
    void InstanceSetup::deviceFeatures( types::device device, VkPhysicalDeviceFeatures &features, void *userPoiner )
    {
        features.samplerAnisotropy = VK_TRUE;
        features.sampleRateShading = VK_TRUE;
    }
    void InstanceSetup::deviceQueueSet( types::device device, queueSet &queues, VkSurfaceKHR surface, void *userPoiner )
    {
        if ( device->data->description->data->queueFamilyProperties.size() - 1 )
        {
            for ( uint32_t i { 0 }; i < device->data->description->data->queueFamilyProperties.size(); i++ )
            {
                VkBool32 presentSupport { false };
                vkGetPhysicalDeviceSurfaceSupportKHR( device->data->description->data->phDevice, i, surface, &presentSupport );
                if ( !device->data->queuesSet.graphic.familyIndex.has_value() && device->data->description->data->queueFamilyProperties[ i ].queueFlags & VK_QUEUE_GRAPHICS_BIT )
                    device->data->queuesSet.graphic = { i, 1.f };
                if ( !device->data->queuesSet.present.familyIndex.has_value() && presentSupport )
                    device->data->queuesSet.present = { i, 1.f };
                else if ( !device->data->queuesSet.transfer.familyIndex.has_value() && device->data->description->data->queueFamilyProperties[ i ].queueFlags & VK_QUEUE_TRANSFER_BIT )
                    device->data->queuesSet.transfer = { i, 1.f };
                // else if ( !queuesSet.compute.familyIndex.has_value() && description.data->queueFamilyProperties[ i ].queueFlags & VK_QUEUE_COMPUTE_BIT )
                //     queuesSet.compute = i;
                else
                    break;
            }
        }
        else
        {
            VkBool32 presentSupport { false };
            vkGetPhysicalDeviceSurfaceSupportKHR( device->data->description->data->phDevice, 0, surface, &presentSupport );
            if ( device->data->description->data->queueFamilyProperties[ 0 ].queueFlags & ( VK_QUEUE_TRANSFER_BIT | VK_QUEUE_GRAPHICS_BIT ) && presentSupport )
            {
                device->data->queuesSet = { { 0, 0, 1.f }, { 0, 0, 1.f }, { 0, 0, 1.f } };
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
        data->window->data->instance->data->setup->deviceQueueSet( this, data->queuesSet, this->data->window->data->surface, data->window->data->instance->data->userPointer );
        std::vector<VkDeviceQueueCreateInfo> QueuesCreateInfo;
        QueuesCreateInfo.reserve( data->queuesSet.getUniqueIndecies().size() );
        auto d { data->queuesSet.getUniqueIndecies() };
        for ( const auto &index : data->queuesSet.getUniqueIndecies() )
        {
            QueuesCreateInfo.push_back( { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO } );
            data->window->data->instance->data->setup->queueFlags( &data->queuesSet, QueuesCreateInfo.back().flags, data->window->data->instance->data->userPointer );
            QueuesCreateInfo.back().queueFamilyIndex = index.first;
            QueuesCreateInfo.back().queueCount       = index.second.second.size();
            QueuesCreateInfo.back().pQueuePriorities = index.second.second.data();
        }
        std::vector<const char *> extensions {};
        VkPhysicalDeviceFeatures2 physicalDeviceFeatures {};
        VkDeviceCreateInfo createInfo {};
        VkPhysicalDeviceFeatures features {};
        data->window->data->instance->data->setup->deviceFeatures( this, features, data->window->data->instance->data->userPointer );
        data->window->data->instance->data->setup->deviceExtensions( this, extensions, data->window->data->instance->data->userPointer );
        data->setExtensions( extensions );
        assert( data->supportExtensions() );

        std::vector<void *> nextChainData;
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        data->window->data->instance->data->setup->deviceNextChain( this, createInfo.pNext, nextChainData, data->window->data->instance->data->userPointer );
        createInfo.queueCreateInfoCount    = static_cast<uint32_t>( QueuesCreateInfo.size() );
        createInfo.pQueueCreateInfos       = QueuesCreateInfo.data();
        createInfo.pEnabledFeatures        = &features;
        createInfo.enabledExtensionCount   = static_cast<uint32_t>( data->extensions.size() );
        createInfo.ppEnabledExtensionNames = data->extensions.size() ? data->extensions.data() : nullptr;
        CHECK_RESULT( vkCreateDevice( data->description->data->phDevice, &createInfo, ALLOCATION_CALLBACK, &data->device ) );
        data->window->data->instance->data->setup->deviceNextChainClear( this, nextChainData, data->window->data->instance->data->userPointer );
        data->queuesSet.init( data->device );
        VkCommandPoolCreateInfo poolCI {};
        poolCI.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCI.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolCI.queueFamilyIndex = data->queuesSet.graphic.familyIndex.value();
        CHECK_RESULT( vkCreateCommandPool( data->device, &poolCI, ALLOCATION_CALLBACK, &data->grapchicPool ) );
        poolCI.queueFamilyIndex = data->queuesSet.transfer.familyIndex.value();
        CHECK_RESULT( vkCreateCommandPool( data->device, &poolCI, ALLOCATION_CALLBACK, &data->transferPool ) );
        poolCI.queueFamilyIndex = data->queuesSet.present.familyIndex.value();
        CHECK_RESULT( vkCreateCommandPool( data->device, &poolCI, ALLOCATION_CALLBACK, &data->presentPool ) );
        // std::vector<descriptorPool> descriptors;
        // data->window->data->instance->data->setup->deviceDescriptorPools( this, data->descriptorPools );
    }

    device::~device()
    {
        vkDestroyCommandPool( data->device, data->grapchicPool, ALLOCATION_CALLBACK );
        vkDestroyCommandPool( data->device, data->transferPool, ALLOCATION_CALLBACK );
        vkDestroyCommandPool( data->device, data->presentPool, ALLOCATION_CALLBACK );
        data->pipelines.clear();
        data->shaders.clear();
        data->layouts.clear();
        data->descriptorPools.clear();
        vkDestroyDevice( data->device, ALLOCATION_CALLBACK );
    }

    types::descriptorPool device::CreatePool( void *userData )
    {
        data->descriptorPools.emplace_back( std::unique_ptr<descriptorPool> { new descriptorPool { this, userData } } );
        return data->descriptorPools.back().get();
    }

    types::shader device::CreateShader( std::string path, std::string main, ShaderStage stage )
    {
        return CreateShader( path.data(), main.data(), stage );
    }

    types::shader device::CreateShader( const char *path, const char *main, ShaderStage stage )
    {
        data->shaders.emplace_back( std::unique_ptr<shader> { new shader { this, path, main, stage } } );
        return data->shaders.back().get();
    }

    types::layout device::CreateLayout( types::descriptorPool pool, void *userData )
    {
        data->layouts.emplace_back( std::unique_ptr<layout> { new layout { this, pool, userData } } );
        return data->layouts.back().get();
    }

    types::pipeline device::CreatePipeline( types::layout layout, std::vector<types::shader> shaders, types::pass pass )
    {
        data->pipelines.emplace_back( std::unique_ptr<pipeline> { new pipeline { this, layout, shaders, pass } } );
        return data->pipelines.back().get();
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
