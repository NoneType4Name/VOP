#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <surface.hxx>
#include <swapchain.hxx>
#include <instance.hxx>

namespace Engine
{
    // void InstanceSetup::surfaceInfo( window::types::window window, const void *&pNext, VkFlags &flags, std::vector<void *> &dataPointers, void *userPoiner ) {}
    // void InstanceSetup::surfaceInfoClear( window::types::window window, std::vector<void *> &dataPointers, void *userPoiner ) {}

    namespace window
    {
        window::window( bool, instance *instance, settings settings ) :
            properties { settings }
        {
            DEFINE_DATA_FIELD( instance );
            data->instance->data->windows.emplace( std::make_pair<types::window, std::vector<Engine::types::swapchain>>( this, {} ) );
            resolution displayRes { getDisplayResolution() };
            auto mode = glfwGetVideoMode( glfwGetPrimaryMonitor() );
            glfwWindowHint( GLFW_RED_BITS, mode->redBits );
            glfwWindowHint( GLFW_GREEN_BITS, mode->greenBits );
            glfwWindowHint( GLFW_BLUE_BITS, mode->blueBits );
            glfwWindowHint( GLFW_REFRESH_RATE, mode->refreshRate );
            glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
            glfwWindowHint( GLFW_RESIZABLE, properties.resize );
            glfwWindowHint( GLFW_DECORATED, properties.decorated );
            glfwWindowHint( GLFW_FLOATING, properties.floating );
            glfwWindowHint( GLFW_VISIBLE, properties.visible );
            if ( !properties.size.width or !properties.size.height )
            {
                glfwWindowHint( GLFW_MAXIMIZED, 1 );
                glfwHandle = glfwCreateWindow( displayRes.width, displayRes.height, properties.title, nullptr, nullptr );
            }
            else
                glfwHandle = glfwCreateWindow( properties.fullScreenRefreshRate ? displayRes.width : properties.size.width, properties.fullScreenRefreshRate ? displayRes.height : properties.size.height, properties.title, properties.fullScreenRefreshRate ? glfwGetPrimaryMonitor() : nullptr, nullptr );
            glfwGetWindowSize( glfwHandle, reinterpret_cast<int *>( const_cast<ENGINE_RESOLUTION_TYPE *>( &properties.size.width ) ), reinterpret_cast<int *>( const_cast<ENGINE_RESOLUTION_TYPE *>( &properties.size.height ) ) );
            if ( properties.fullScreenRefreshRate )
                glfwSetWindowMonitor( glfwHandle, glfwGetPrimaryMonitor(), 0, 0, properties.size.width, properties.size.height, properties.fullScreenRefreshRate );
            glfwSetWindowUserPointer( glfwHandle, this );
            glfwSetFramebufferSizeCallback( glfwHandle, []( GLFWwindow *wnd, int w, int h )
                                            {auto from_wnd    = reinterpret_cast<window *>( glfwGetWindowUserPointer( wnd ) );
                                        const_cast<struct settings*>(&from_wnd->properties)->size.width = static_cast<ENGINE_RESOLUTION_TYPE>( w );
                                        const_cast<struct settings*>(&from_wnd->properties)->size.height = static_cast<ENGINE_RESOLUTION_TYPE>( h );
                                        from_wnd->resizeCallBack(w, h); } );
            glfwSetKeyCallback( glfwHandle, []( GLFWwindow *wnd, int key, int scancode, int action, int mods )
                                {   
                                    auto from_wnd    = reinterpret_cast<window *>( glfwGetWindowUserPointer( wnd ) );
                                    from_wnd->eventCallBack( key, scancode, action, mods ); } );
        }

        window::window( instance *instance, settings settings ) :
            window( 1, instance, settings )
        {
            data->createSurface( 0, 0 );
        }

        window::~window()
        {
            auto i { data->swapchains.begin() };
            while ( i != data->swapchains.end() )
                delete *i++;
            data->instance->data->windows.erase( this );
            vkDestroySurfaceKHR( data->instance->handle, surface, ALLOCATION_CALLBACK );
            glfwDestroyWindow( glfwHandle );
        }

        window::DATA_TYPE::DATA_TYPE( types::window parent, struct instance *instance ) :
            parent { parent }, instance { instance }
        {
        }

        window::DATA_TYPE::~DATA_TYPE()
        {
        }

        Engine::types::swapchain window::getLink( Engine::types::device device )
        {
            for ( auto &swp : data->swapchains )
            {
                if ( swp->data->device == device )
                    return swp;
            }
            return nullptr;
        }

        resolution window::getDisplayResolution()
        {
            auto mode = glfwGetVideoMode( glfwGetPrimaryMonitor() );
            return { static_cast<ENGINE_RESOLUTION_TYPE>( mode->width ), static_cast<ENGINE_RESOLUTION_TYPE>( mode->height ) };
        }

        void window::setTitle( const char *title )
        {
            *const_cast<const char **>( &properties.title ) = title;
            glfwSetWindowTitle( glfwHandle, properties.title );
        }

        void window::updateProperties( settings properties )
        {
            *const_cast<settings *>( &this->properties )         = properties;
            const_cast<settings *>( &this->properties )->visible = 1;

            if ( this->properties.fullScreenRefreshRate )
            {
                const_cast<settings *>( &this->properties )->size = getDisplayResolution();
                glfwSetWindowMonitor( glfwHandle, glfwGetPrimaryMonitor(), 0, 0, this->properties.size.width, this->properties.size.height, this->properties.fullScreenRefreshRate );
            }
            else
            {
                glfwSetWindowMonitor( glfwHandle, 0, 0, 0, this->properties.size.width, this->properties.size.height, GLFW_DONT_CARE );
            }
            glfwSetWindowAttrib( glfwHandle, GLFW_RESIZABLE, this->properties.resize );
            glfwSetWindowAttrib( glfwHandle, GLFW_DECORATED, this->properties.decorated );
            glfwSetWindowAttrib( glfwHandle, GLFW_FLOATING, this->properties.floating );
        }

        void window::eventCallBack( int key, int scancode, int action, int mods )
        {
        }

        void window::resizeCallBack( int width, int height )
        {
        }

        void window::updateEvents()
        {
            glfwPollEvents();
        }

        bool window::shouldClose()
        {
            return glfwWindowShouldClose( glfwHandle );
        }
    } // namespace window
} // namespace Engine