#include "WindowsPlatform.hxx"
#include <common/logging.hxx>

namespace Engine
{
    namespace
    {
        namespace tools
        {
            GLFWwindow *_window{ nullptr };
            VkSurfaceKHR _surface{ nullptr };

            void createWindow();

            VkSurfaceKHR createSurface( VkInstance instance )
            {
                if( _window == nullptr )
                    window::create();
                VkWin32SurfaceCreateInfoKHR CI{ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR, 0, 0, GetModuleHandle( nullptr ), glfwGetWin32Window( _window ) };
                CHECK_RESULT( vkCreateWin32SurfaceKHR( instance, &CI, ALLOCATION_CALLBACK, &_surface ) );
                return _surface;
            };

            VkSurfaceKHR getSurface()
            {
                return _surface;
            };
        } // namespace tools
    }     // namespace
    namespace window
    {
        namespace
        {
            RESOLUTION_TYPE _width{ 0 };
            RESOLUTION_TYPE _height{ 0 };
            ResizeCallback _resizeCallBack{ nullptr };
            EventCallBack _eventCallBack{ nullptr };
        } // namespace

        resolution getResolution()
        {
            return { _width, _height };
        }

        resolution getDisplayResolution()
        {
            GLFWmonitor *_monitor{ nullptr };
            if( tools::_window == nullptr )
                _monitor = glfwGetPrimaryMonitor();
            else
                _monitor = glfwGetWindowMonitor( tools::_window );
            glfwGetVideoMode( _monitor );
            auto mode = glfwGetVideoMode( _monitor );
            return { static_cast<RESOLUTION_TYPE>( mode->width ), static_cast<RESOLUTION_TYPE>( mode->height ) };
        }

        void create()
        {
        }

        void cenralize()
        {
            auto monitor_resolution{ getDisplayResolution() };
            auto window_resolution{ getResolution() };
            glfwSetWindowPos( tools::_window, ( monitor_resolution.width / 2 ) - ( window_resolution.width / 2 ), ( monitor_resolution.height / 2 ) - ( window_resolution.height / 2 ) );
        }

        void setTitle( const char *title )
        {
            glfwSetWindowTitle( tools::_window, title );
        }

        void setTitle( std::string title )
        {
            glfwSetWindowTitle( tools::_window, title.c_str() );
        }

        void setWindowResolution( RESOLUTION_TYPE width, RESOLUTION_TYPE height )
        {
            glfwSetWindowSize( tools::_window, width, height );
            _resizeCallBack( width, height );
        }

        void setResizeCallBack( ResizeCallback callback )
        {
            _resizeCallBack = callback;
            glfwSetWindowSizeCallback( tools::_window, []( GLFWwindow *wnd, int w, int h )
                                       {_width = static_cast<RESOLUTION_TYPE>( w );
                                        _height = static_cast<RESOLUTION_TYPE>( h );
                                        _resizeCallBack(w, h); } );
        }

        void setKeyEventsCallback( EventCallBack callback )
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