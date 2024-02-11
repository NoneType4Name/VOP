#include <surface.hxx>
#include <swapchain.hxx>
#include <instance.hxx>
#include <common/logging.hxx>

namespace Engine
{
    surface::surface( bool, instance *instance, uint32_t width, uint32_t height, VkSurfaceKHR surface ) :
        handle { surface }
    {
        DEFINE_DATA_FIELD( instance, width, height );
        data->instance->data->surfaces[ this ] = {};
    }

    surface::surface( instance *instance, uint32_t width, uint32_t height, VkSurfaceKHR handle ) :
        surface( 1, instance, width, height, handle )
    {
    }

    surface::~surface()
    {
        auto i { data->swapchains.begin() };
        while ( i != data->swapchains.end() )
            delete *i++;
        data->instance->data->surfaces.erase( this );
        vkDestroySurfaceKHR( data->instance->handle, handle, ENGINE_ALLOCATION_CALLBACK );
    }

    surface::DATA_TYPE::DATA_TYPE( types::surface parent, struct instance *instance, uint32_t width, uint32_t height ) :
        parent { parent }, instance { instance }, width { width }, height { height }
    {
    }

    surface::DATA_TYPE::~DATA_TYPE()
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