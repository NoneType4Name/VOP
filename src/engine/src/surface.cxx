#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <surface.hxx>
#include <EHI.hxx>

namespace Engine
{
    // void InstanceSetup::surfaceInfo( window::types::window window, const void *&pNext, VkFlags &flags, std::vector<void *> &dataPointers, void *userPoiner ) {}
    // void InstanceSetup::surfaceInfoClear( window::types::window window, std::vector<void *> &dataPointers, void *userPoiner ) {}

    namespace window
    {
        window::window() = default;
        window::window( Engine::instance *instance, settings settings )
        {
            DEFINE_DATA_FIELD;
            data->instance = instance;
            data->settings = settings;
            resolution displayRes { data->parent->getDisplayResolution() };
            glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
            data->window = glfwCreateWindow( settings.fullScreen ? displayRes.width : data->settings.size.width, !data->settings.fullScreen ? displayRes.height : data->settings.size.height, data->settings.title.c_str(), data->settings.fullScreen ? glfwGetPrimaryMonitor() : nullptr, nullptr );
            glfwSetWindowUserPointer( data->window, this );
            glfwGetWindowSize( data->window, reinterpret_cast<int *>( &this->data->settings.size.width ), reinterpret_cast<int *>( &this->data->settings.size.height ) );
            glfwSetFramebufferSizeCallback( data->window, []( GLFWwindow *wnd, int w, int h )
                                            {auto from_wnd    = reinterpret_cast<window *>( glfwGetWindowUserPointer( wnd ) );
                                        from_wnd->data->width = static_cast<ENGINE_RESOLUTION_TYPE>( w );
                                        from_wnd->data->height = static_cast<ENGINE_RESOLUTION_TYPE>( h );
                                        from_wnd->data->resizeCallBack(w, h); } );
            glfwSetKeyCallback( data->window, []( GLFWwindow *wnd, int key, int scancode, int action, int mods )
                                {   
                                    auto from_wnd    = reinterpret_cast<window *>( glfwGetWindowUserPointer( wnd ) );
                                    if (from_wnd->data->eventCallBack)
                                        from_wnd->data->eventCallBack( key, scancode, action, mods ); } );
        }
        void window::DATA_TYPE::init()
        {
            const void *next { nullptr };
            std::vector<void *> nextChainData;
            VkFlags flags {};
            data->instance->data->setup->surfaceInfo( this, next, flags, nextChainData, data->instance->data->userPointer );
            data->createSurface( data->instance->data->handle, next, flags );
            data->instance->data->setup->surfaceInfoClear( this, nextChainData, data->instance->data->userPointer );
            // todo: fix bug with full screen window.
        }
        window::~window()
        {
            data->destroySurface( data->instance->data->handle );
        }

        resolution window::getResolution()
        {
            return { data->width, data->height };
        }

        resolution window::getDisplayResolution()
        {
            auto mode = glfwGetVideoMode( glfwGetPrimaryMonitor() );
            return { static_cast<ENGINE_RESOLUTION_TYPE>( mode->width ), static_cast<ENGINE_RESOLUTION_TYPE>( mode->height ) };
        }

        void window::setTitle( const char *title )
        {
            data->title = title;
            glfwSetWindowTitle( data->window, title );
        }

        void window::setTitle( std::string title )
        {
            setTitle( title.c_str() );
        }

        void window::setWindowResolution( ENGINE_RESOLUTION_TYPE width, ENGINE_RESOLUTION_TYPE height )
        {
            resolution displayRes { getDisplayResolution() };
            if ( width + height )
            {
                glfwSetWindowAttrib( data->window, GLFW_RESIZABLE, GLFW_TRUE );
                glfwSetWindowAttrib( data->window, GLFW_DECORATED, GLFW_TRUE );
            }
            else
            {
                glfwSetWindowAttrib( data->window, GLFW_RESIZABLE, GLFW_FALSE );
                glfwSetWindowAttrib( data->window, GLFW_DECORATED, GLFW_FALSE );
            }
            if ( !width ) width = displayRes.width;
            if ( !height ) height = displayRes.height;
            data->width  = width;
            data->height = height;
            glfwSetWindowSize( data->window, static_cast<int>( width ), static_cast<int>( height ) );
            if ( data->resizeCallBack ) data->resizeCallBack( width, height );
        }

        // void window::setResizeCallBack( ResizeCallback callback )
        // {
        //     data->resizeCallBack = callback;
        // }

        // void window::setKeyEventsCallback( KeyEventCallBack callback )
        // {
        //     data->eventCallBack = callback;
        // }

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