#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <surface.hxx>
#include <swapchain.hxx>
#include <EHI.hxx>

namespace Engine
{
    // void InstanceSetup::surfaceInfo( window::types::window window, const void *&pNext, VkFlags &flags, std::vector<void *> &dataPointers, void *userPoiner ) {}
    // void InstanceSetup::surfaceInfoClear( window::types::window window, std::vector<void *> &dataPointers, void *userPoiner ) {}

    namespace window
    {
        window::window( bool, instance *instance, settings settings ) :
            properties { settings }
        {
            construct( instance, settings );
        }

        window::window( instance *instance, settings settings ) :
            window( 1, instance, settings )
        {
            setup( instance, settings );
        }

        window::~window()
        {
            auto sI { data->swapchains.begin() };
            while ( sI != data->swapchains.end() )
                delete *sI++;
            data->instance->data->windows.erase( this );
            data->destroySurface( data->instance->data->handle );
            glfwDestroyWindow( data->window );
        }

        window::DATA_TYPE::DATA_TYPE( types::window parent, struct instance *instance ) :
            parent { parent }, instance { instance }
        {
        }

        window::DATA_TYPE::~DATA_TYPE()
        {
        }

        void window::construct( instance *instance, settings settings )
        {
            const_cast<std ::unique_ptr<data_T> &>( data ) = std ::make_unique<data_T>( this, instance );
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
                data->window = glfwCreateWindow( displayRes.width, displayRes.height, properties.title, nullptr, nullptr );
            }
            else
                data->window = glfwCreateWindow( properties.fullScreenRefreshRate ? displayRes.width : properties.size.width, properties.fullScreenRefreshRate ? displayRes.height : properties.size.height, properties.title, properties.fullScreenRefreshRate ? glfwGetPrimaryMonitor() : nullptr, nullptr );
            glfwGetWindowSize( data->window, reinterpret_cast<int *>( const_cast<ENGINE_RESOLUTION_TYPE *>( &properties.size.width ) ), reinterpret_cast<int *>( const_cast<ENGINE_RESOLUTION_TYPE *>( &properties.size.height ) ) );
            if ( properties.fullScreenRefreshRate )
                glfwSetWindowMonitor( data->window, glfwGetPrimaryMonitor(), 0, 0, properties.size.width, properties.size.height, properties.fullScreenRefreshRate );
            glfwSetWindowUserPointer( data->window, this );
            glfwSetFramebufferSizeCallback( data->window, []( GLFWwindow *wnd, int w, int h )
                                            {auto from_wnd    = reinterpret_cast<window *>( glfwGetWindowUserPointer( wnd ) );
                                        const_cast<struct settings*>(&from_wnd->properties)->size.width = static_cast<ENGINE_RESOLUTION_TYPE>( w );
                                        const_cast<struct settings*>(&from_wnd->properties)->size.height = static_cast<ENGINE_RESOLUTION_TYPE>( h );
                                        from_wnd->resizeCallBack(w, h); } );
            glfwSetKeyCallback( data->window, []( GLFWwindow *wnd, int key, int scancode, int action, int mods )
                                {   
                                    auto from_wnd    = reinterpret_cast<window *>( glfwGetWindowUserPointer( wnd ) );
                                    from_wnd->eventCallBack( key, scancode, action, mods ); } );
        }

        void window::setup( instance *instance, settings settings )
        {
            data->createSurface( instance->data->handle, 0, 0 );
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
            glfwSetWindowTitle( data->window, properties.title );
        }

        void window::updateProperties( settings properties )
        {
            *const_cast<settings *>( &this->properties )         = properties;
            const_cast<settings *>( &this->properties )->visible = 1;

            if ( this->properties.fullScreenRefreshRate )
            {
                const_cast<settings *>( &this->properties )->size = getDisplayResolution();
                glfwSetWindowMonitor( data->window, glfwGetPrimaryMonitor(), 0, 0, this->properties.size.width, this->properties.size.height, this->properties.fullScreenRefreshRate );
            }
            else
            {
                glfwSetWindowMonitor( data->window, 0, 0, 0, this->properties.size.width, this->properties.size.height, GLFW_DONT_CARE );
            }
            glfwSetWindowAttrib( data->window, GLFW_RESIZABLE, this->properties.resize );
            glfwSetWindowAttrib( data->window, GLFW_DECORATED, this->properties.decorated );
            glfwSetWindowAttrib( data->window, GLFW_FLOATING, this->properties.floating );
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
            return glfwWindowShouldClose( data->window );
        }
    } // namespace window
} // namespace Engine