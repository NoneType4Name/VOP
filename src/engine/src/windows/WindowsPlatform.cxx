#include "WindowsPlatform.hxx"
#include <common/logging.hxx>
#include <RHI.hxx>
#include <engine.hxx>

namespace Engine
{
    namespace tools
    {
        namespace
        {
            GLFWwindow *_window{ nullptr };
            VkSurfaceKHR _surface{ nullptr };
        } // namespace

        void createSurface( VkInstance instance )
        {
            VkWin32SurfaceCreateInfoKHR CI{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR, 0, 0, GetModuleHandle( nullptr ), glfwGetWin32Window( _window ) };
            CHECK_RESULT( vkCreateWin32SurfaceKHR( instance, &CI, ALLOCATION_CALLBACK, &_surface ) );
        };

        void createWindow( RESOLUTION_TYPE width, RESOLUTION_TYPE height, const char *title )
        {
            window::resolution displayRes{ window::getDisplayResolution() };
            glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
            glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );
            _window = glfwCreateWindow( width ? width : displayRes.width, height ? height : displayRes.height, title, nullptr, nullptr );
            window::setTitle( title );
            // window::setWindowResolution( width, height );
        }

        void destroySurface()
        {
            vkDestroySurfaceKHR( getInstance(), _surface, ALLOCATION_CALLBACK );
        }

        void destroyWindow()
        {
            glfwDestroyWindow( _window );
            glfwTerminate();
        }

        const VkSurfaceKHR getSurface()
        {
            return _surface;
        };

        GLFWwindow *getWindow()
        {
            return _window;
        }

    } // namespace tools
    namespace window
    {
        namespace
        {
            RESOLUTION_TYPE _width{ 0 };
            RESOLUTION_TYPE _height{ 0 };
            std::string _title{};
            ResizeCallback _resizeCallBack{ nullptr };
            KeyEventCallBack _eventCallBack{ nullptr };
        } // namespace

        resolution getResolution()
        {
            return { _width, _height };
        }

        resolution getDisplayResolution()
        {
            auto mode = glfwGetVideoMode( glfwGetPrimaryMonitor() );
            return { static_cast<RESOLUTION_TYPE>( mode->width ), static_cast<RESOLUTION_TYPE>( mode->height ) };
        }

        void cenralize()
        {
            auto monitor_resolution{ getDisplayResolution() };
            auto window_resolution{ getResolution() };
            glfwSetWindowPos( tools::_window, ( monitor_resolution.width / 2 ) - ( window_resolution.width / 2 ), ( monitor_resolution.height / 2 ) - ( window_resolution.height / 2 ) );
        }

        void setTitle( const char *title )
        {
            _title = title;
            glfwSetWindowTitle( tools::_window, title );
        }

        void setTitle( std::string title )
        {
            setTitle( title.c_str() );
        }

        void setWindowResolution( RESOLUTION_TYPE width, RESOLUTION_TYPE height )
        {
            resolution displayRes{ window::getDisplayResolution() };
            if( width + height )
            {
                glfwSetWindowAttrib( tools::_window, GLFW_RESIZABLE, GLFW_TRUE );
                glfwSetWindowAttrib( tools::_window, GLFW_DECORATED, GLFW_TRUE );
            }
            else
            {

                glfwSetWindowAttrib( tools::_window, GLFW_RESIZABLE, GLFW_FALSE );
                glfwSetWindowAttrib( tools::_window, GLFW_DECORATED, GLFW_FALSE );
            }
            if( !width ) width = displayRes.width;
            if( !height ) height = displayRes.height;
            _width  = width;
            _height = height;
            glfwSetWindowSize( tools::_window, static_cast<int>( width ), static_cast<int>( height ) );
            if( _resizeCallBack ) _resizeCallBack( width, height );
        }

        void setResizeCallBack( ResizeCallback callback )
        {
            _resizeCallBack = callback;
            glfwSetWindowSizeCallback( tools::_window, []( GLFWwindow *wnd, int w, int h )
                                       {_width = static_cast<RESOLUTION_TYPE>( w );
                                        _height = static_cast<RESOLUTION_TYPE>( h );
                                        _resizeCallBack(w, h); } );
        }

        void setKeyEventsCallback( KeyEventCallBack callback )
        {
            _eventCallBack = callback;
            glfwSetKeyCallback( tools::_window, []( GLFWwindow *wnd, int key, int scancode, int action, int mods )
                                { _eventCallBack( key, scancode, action, mods ); } );
        }

        void updateEvents()
        {
            glfwPollEvents();
        }

        bool shouldClose()
        {
            return glfwWindowShouldClose( tools::_window );
        }
    } // namespace window
} // namespace Engine
