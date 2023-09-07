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
        } // namespace

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
    } // namespace tools
} // namespace Engine
