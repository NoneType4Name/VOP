#include <ObjectiveVulkan/surface.hxx>
#include <ObjectiveVulkan/swapchain.hxx>
#include <ObjectiveVulkan/instance.hxx>
#include <ObjectiveVulkan/common/logging.hxx>

namespace Engine
{
    surface::surface( instance *instance, uint32_t width, uint32_t height )
    {
        OBJECTIVE_VULKAN_OBJECTIVE_VULKAN_DEFINE_DATA_FIELD( instance, width, height );
        data->instance->data->surfaces[ this ] = {};
    }

    surface::~surface()
    {
        auto i { data->swapchains.begin() };
        while ( i != data->swapchains.end() )
            delete *i++;
        data->instance->data->surfaces.erase( this );
        vkDestroySurfaceKHR( data->instance->handle, handle, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
    }

    surface::OBJECTIVE_VULKAN_DATA_TYPE::OBJECTIVE_VULKAN_DATA_TYPE( types::surface parent, struct instance *instance, uint32_t width, uint32_t height ) :
        parent { parent }, instance { instance }, width { width }, height { height }
    {
    }

    surface::OBJECTIVE_VULKAN_DATA_TYPE::~OBJECTIVE_VULKAN_DATA_TYPE()
    {
    }

    void surface::updateResolution( uint32_t width, uint32_t height )
    {
        data->width  = width;
        data->height = height;
        for ( const auto &swp : data->swapchains )
        {
            uint32_t c;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR( swp->data->device->data->description->data->phDevice, handle, &swp->properties.capabilities );
            vkGetPhysicalDeviceSurfaceFormatsKHR( swp->data->device->data->description->data->phDevice, handle, &c, nullptr );
            swp->properties.formats.resize( c );
            vkGetPhysicalDeviceSurfaceFormatsKHR( swp->data->device->data->description->data->phDevice, handle, &c, swp->properties.formats.data() );
            vkGetPhysicalDeviceSurfacePresentModesKHR( swp->data->device->data->description->data->phDevice, handle, &c, nullptr );
            swp->properties.presentModes.resize( c );
            vkGetPhysicalDeviceSurfacePresentModesKHR( swp->data->device->data->description->data->phDevice, handle, &c, swp->properties.presentModes.data() );
        }
    }

    types::swapchain surface::getLink( Engine::types::device device )
    {
        for ( auto &swp : data->swapchains )
        {
            if ( swp->data->device == device )
                return swp;
        }
        return nullptr;
    }
} // namespace Engine