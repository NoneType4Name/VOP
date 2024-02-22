#include <ObjectiveVulkan/instance.hxx>
#include <ObjectiveVulkan/surface.hxx>
#include <ObjectiveVulkan/device.hxx>
#include <ObjectiveVulkan/common/logging.hxx>
#define OBJECTIVE_VULKAN_VERSION VK_MAKE_VERSION( 0, 0, 1 )

namespace Engine
{
    instance::instance( const char *appName, uint32_t appVersion )
    {
        OBJECTIVE_VULKAN_OBJECTIVE_VULKAN_DEFINE_DATA_FIELD();
    }

    instance::~instance()
    {
        auto sI { data->surfaces.begin() };
        while ( sI != data->surfaces.end() )
        {
            auto swI { sI->second.begin() };
            while ( swI != sI->second.end() )
                delete *swI++;
            delete ( sI++ )->first;
        }
        auto dI { data->devices.begin() };
        while ( dI != data->devices.end() )
            delete *dI++;
        auto dDs { data->deviceDescriptions.begin() };
        while ( dDs != data->deviceDescriptions.end() )
            delete *dDs++;
        data->destroyDebugLayerCallback();
        vkDestroyInstance( handle, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
    }

    void instance::OBJECTIVE_VULKAN_DATA_TYPE::create( VkInstanceCreateInfo createInfo )
    {
        const_cast<VkApplicationInfo *>( createInfo.pApplicationInfo )->engineVersion = OBJECTIVE_VULKAN_VERSION;
        const_cast<VkApplicationInfo *>( createInfo.pApplicationInfo )->pEngineName   = "NoneTypeName.";
        std::vector<const char *> _layers { createInfo.ppEnabledLayerNames, &createInfo.ppEnabledLayerNames[ createInfo.enabledLayerCount ] };
        std::vector<const char *> _extensions { createInfo.ppEnabledExtensionNames, &createInfo.ppEnabledExtensionNames[ createInfo.enabledExtensionCount ] };
        setLayers( _layers );
        setExtensions( _extensions );
        assert( supportExtensions() );
        assert( supportLayers() );
        createInfo.enabledLayerCount       = layers.size();
        createInfo.ppEnabledLayerNames     = layers.data();
        createInfo.enabledExtensionCount   = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();
        OBJECTIVE_VULKAN_CHECK_RESULT( vkCreateInstance( &createInfo, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &parent->handle ) );
    }
} // namespace Engine