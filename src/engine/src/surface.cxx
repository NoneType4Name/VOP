#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <surface.hxx>
#include <EHI.hxx>

namespace Engine
{
    namespace window
    {
        void window::DATA_TYPE::setupNextChain( const void *&pNext, std::vector<void *> &dataPointers ) {}
        void window::DATA_TYPE::setupFlags( VkWin32SurfaceCreateFlagsKHR flags ) {}
        window::window()  = default;
        window::~window() = default;
        window::window( RESOLUTION_TYPE width, RESOLUTION_TYPE height, const char *title, instance *instance )
        {
            // data       = new data;
            DEFINE_DATA_FIELD;
            resolution displayRes { getDisplayResolution() };
            glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
            glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );
            data->window = glfwCreateWindow( width ? width : displayRes.width, height ? height : displayRes.height, title, nullptr, nullptr );
            glfwSetWindowUserPointer( data->window, this );
            const void *next;
            std::vector<void *> nextChainData;
            VkWin32SurfaceCreateFlagsKHR flags;
            data->setupNextChain( next, nextChainData );
            data->setupFlags( flags );
            data->createSurface( instance->data->handle, &next, flags );
            cenralize();
            setWindowResolution( width, height );
        }

        resolution window::getResolution()
        {
            return { data->width, data->height };
        }

        resolution window::getDisplayResolution()
        {
            auto mode = glfwGetVideoMode( glfwGetPrimaryMonitor() );
            return { static_cast<RESOLUTION_TYPE>( mode->width ), static_cast<RESOLUTION_TYPE>( mode->height ) };
        }

        void window::cenralize()
        {
            auto monitor_resolution { getDisplayResolution() };
            auto window_resolution { getResolution() };
            glfwSetWindowPos( data->window, ( monitor_resolution.width / 2 ) - ( window_resolution.width / 2 ), ( monitor_resolution.height / 2 ) - ( window_resolution.height / 2 ) );
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

        void window::setWindowResolution( RESOLUTION_TYPE width, RESOLUTION_TYPE height )
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

        void window::setResizeCallBack( ResizeCallback callback )
        {
            data->resizeCallBack = callback;
            glfwSetWindowSizeCallback( data->window, []( GLFWwindow *wnd, int w, int h )
                                       {auto from_wnd    = reinterpret_cast<window *>( glfwGetWindowUserPointer( wnd ) );
                                        from_wnd->data->width = static_cast<RESOLUTION_TYPE>( w );
                                        from_wnd->data->height = static_cast<RESOLUTION_TYPE>( h );
                                        from_wnd->data->resizeCallBack(w, h); } );
        }

        void window::setKeyEventsCallback( KeyEventCallBack callback )
        {
            data->eventCallBack = callback;
            glfwSetKeyCallback( data->window, []( GLFWwindow *wnd, int key, int scancode, int action, int mods )
                                {   
                                    auto from_wnd    = reinterpret_cast<window *>( glfwGetWindowUserPointer( wnd ) );
                                    from_wnd->data->eventCallBack( key, scancode, action, mods ); } );
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