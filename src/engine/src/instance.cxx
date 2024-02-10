#include <instance.hxx>
#include <surface.hxx>
#include <device.hxx>
#include <common/logging.hxx>

namespace Engine
{
    instance::instance( bool, const char *appName, uint32_t appVersion )
    {
        DEFINE_DATA_FIELD();
    }

    instance::instance( const char *appName, uint32_t appVersion ) :
        instance { 1, appName, appVersion }
    {
        VkApplicationInfo ApplicationInfo {};
        ApplicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        ApplicationInfo.apiVersion         = VK_API_VERSION_1_0;
        ApplicationInfo.pApplicationName   = appName;
        ApplicationInfo.applicationVersion = appVersion;

        VkDebugUtilsMessengerCreateInfoEXT debugUtilsMsgCI { data->setupDebugLayerCallback() };
        std::vector<VkValidationFeatureEnableEXT> enabledVFeatures { VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT };
        VkValidationFeaturesEXT ValidationFeatures {};
        ValidationFeatures.sType                          = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        ValidationFeatures.enabledValidationFeatureCount  = enabledVFeatures.size();
        ValidationFeatures.pEnabledValidationFeatures     = enabledVFeatures.data();
        ValidationFeatures.disabledValidationFeatureCount = 0;
        ValidationFeatures.pDisabledValidationFeatures    = nullptr;
        debugUtilsMsgCI.pNext                             = &ValidationFeatures;

        std::vector<const char *> ext { VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
        std::vector<const char *> lays {};

        VkInstanceCreateInfo InstanceCreateInfo {};
        InstanceCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        InstanceCreateInfo.pNext                   = &debugUtilsMsgCI;
        InstanceCreateInfo.enabledLayerCount       = lays.size();
        InstanceCreateInfo.ppEnabledLayerNames     = lays.data();
        InstanceCreateInfo.enabledExtensionCount   = ext.size();
        InstanceCreateInfo.ppEnabledExtensionNames = ext.data();
        InstanceCreateInfo.pApplicationInfo        = &ApplicationInfo;
        data->create( InstanceCreateInfo );
        debugUtilsMsgCI.pNext = nullptr;
        data->initDebugLayerCallBack( debugUtilsMsgCI );
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
        vkDestroyInstance( handle, ENGINE_ALLOCATION_CALLBACK );
    }

    void instance::DATA_TYPE::create( VkInstanceCreateInfo createInfo )
    {
        const_cast<VkApplicationInfo *>( createInfo.pApplicationInfo )->engineVersion = ENGINE_VERSION;
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
        CHECK_RESULT( vkCreateInstance( &createInfo, ENGINE_ALLOCATION_CALLBACK, &parent->handle ) );
    }
} // namespace Engine