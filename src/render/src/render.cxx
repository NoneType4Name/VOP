#include <render.hxx>

namespace
{
    struct _
    {
        _()
        {
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
        }
    } _;
} // namespace

namespace render
{
    namespace
    {
        uint16_t width{ 800ui16 };
        uint16_t height{ 600ui16 };
        uint16_t DisplayWidth;
        uint16_t DisplayHeight;
        std::string title;
        GLFWwindow *window{ nullptr };
        GLFWmonitor *monitor{ nullptr };
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

    } // namespace

    void RENDER_EXPORT init( uint16_t width, uint16_t height, const char *title )
    {
        window = glfwCreateWindow( width, height, title, nullptr, nullptr );
    }

    void SetWindowResolution( uint16_t width, uint16_t height )
    {
        _getScreenResolution( DisplayWidth, DisplayHeight );
        if( !width ) width = DisplayWidth;
        else if( !height )
            height = DisplayHeight;
        else if( width + height )
        {
            glfwSetWindowAttrib( window, GLFW_RESIZABLE, GLFW_TRUE );
            glfwSetWindowAttrib( window, GLFW_DECORATED, GLFW_TRUE );
        }
        else
        {

            glfwSetWindowAttrib( window, GLFW_RESIZABLE, GLFW_FALSE );
            glfwSetWindowAttrib( window, GLFW_DECORATED, GLFW_FALSE );
        }

        glfwSetWindowSize( window, static_cast<int>( width ), static_cast<int>( height ) );
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
} // namespace render