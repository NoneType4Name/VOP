#include <engine.hxx>

namespace
{
    struct _
    {
        _()
        {
            spdlog::set_level( DEBUG ? spdlog::level::trace : spdlog::level::critical );
            spdlog::set_pattern( "[%H:%M:%S.%e] [%^%l%$] %v" );
            if( glfwInit() )
            {
                SPDLOG_DEBUG( "GLFW{} inititialized.", glfwGetVersionString() );
                glfwSetErrorCallback( []( int code, const char *data )
                                      { SPDLOG_ERROR( "GLFW ERROR {}: {}", code, data ); } );
            }
            else
            {
                SPDLOG_CRITICAL( "GLFW not initialized." );
                SPDLOG_INFO( "Exit with code {}.", EXIT_FAILURE );
                exit( EXIT_FAILURE );
            }
        }
        ~_()
        {
            glfwTerminate();
        }
    } _;
} // namespace

namespace Engine
{
    namespace
    {
        uint16_t _width{ 800ui16 };
        uint16_t _height{ 600ui16 };
        uint16_t DisplayWidth;
        uint16_t DisplayHeight;
        std::string title;
        GLFWwindow *window{ nullptr };
        GLFWmonitor *monitor{ nullptr };
        KeyEventCallBack _KeyEventCallBack{ nullptr };
        void _getScreenResolution( uint16_t &width, uint16_t &height )
        {
            if( monitor == nullptr )
                monitor = glfwGetPrimaryMonitor();
            else
                monitor = glfwGetWindowMonitor( window );
            glfwGetVideoMode( monitor );
            auto mode = glfwGetVideoMode( monitor );
            width     = mode->width;
            height    = mode->height;
        }
        void FramebufferResizeCallback( GLFWwindow *window, int w, int h )
        {
            SetWindowResolution( w, h );
            // TO DO: update frame buffer size (recreate swapchain).
        }

        inline void WindwoResizeCallback( GLFWwindow *window, int w, int h )
        {
            FramebufferResizeCallback( window, w, h );
        }

        void _KeyCallbackManager( GLFWwindow *wnd, int key, int scancode, int action, int mods )
        {
            _KeyEventCallBack( key, scancode, action, mods );
        }

    } // namespace

    void WindowInit( uint16_t width, uint16_t height, const char *title )
    {
        glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
        glfwWindowHint( GLFW_RESIZABLE, GLFW_TRUE );
        window = glfwCreateWindow( width ? width : _width, height ? height : _height, title, nullptr, nullptr );
        SetWindowResolution( width, height );
        glfwSetFramebufferSizeCallback( window, FramebufferResizeCallback );
        glfwSetWindowSizeCallback( window, WindwoResizeCallback );
        CentralizeWindow();
    }

    void WindowDestroy()
    {
        glfwDestroyWindow( window );
    }

    void SetWindowResolution( uint16_t width, uint16_t height )
    {
        _getScreenResolution( DisplayWidth, DisplayHeight );
        if( width + height )
        {
            glfwSetWindowAttrib( window, GLFW_RESIZABLE, GLFW_TRUE );
            glfwSetWindowAttrib( window, GLFW_DECORATED, GLFW_TRUE );
        }
        else
        {

            glfwSetWindowAttrib( window, GLFW_RESIZABLE, GLFW_FALSE );
            glfwSetWindowAttrib( window, GLFW_DECORATED, GLFW_FALSE );
        }
        if( !width ) width = DisplayWidth;
        if( !height ) height = DisplayHeight;
        _width  = width;
        _height = height;
        glfwSetWindowSize( window, static_cast<int>( width ), static_cast<int>( height ) );
    }

    void CentralizeWindow()
    {
        glfwSetWindowPos( window, ( DisplayWidth / 2 ) - ( _width / 2 ), ( DisplayHeight / 2 ) - ( _height / 2 ) );
    }

    void SetWindowTitle( const char *new_title )
    {
        title = new_title;
        glfwSetWindowTitle( window, new_title );
    }
    inline void SetWindowTitle( std::string &new_title )
    {
        SetWindowTitle( new_title.c_str() );
    }

    KeyEventCallBack SetKeyEventsCallback( KeyEventCallBack Callback )
    {
        KeyEventCallBack prvsClbck{ _KeyEventCallBack };
        glfwSetKeyCallback( window, _KeyCallbackManager );
        _KeyEventCallBack = Callback;
        return _KeyEventCallBack;
    }

    inline bool WindowShouldClose()
    {
        return glfwWindowShouldClose( window );
    }
    inline void UpdateEvents()
    {
        glfwPollEvents();
    }

} // namespace Engine
