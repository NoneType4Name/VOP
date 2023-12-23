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
        window::window()
        {
        }

        window::window( Engine::instance *instance, struct settings settings ) :
            properties { settings }
        {
            DEFINE_DATA_FIELD;
            data->instance = instance;
            resolution displayRes { getDisplayResolution() };
            glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
            glfwWindowHint( GLFW_RESIZABLE, properties.resize );
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

        void window::setup()
        {
            data->createSurface( data->instance->data->handle, 0, 0 );
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

        void window::setResolution( ENGINE_RESOLUTION_TYPE width, ENGINE_RESOLUTION_TYPE height, int fullScreenRefreshRate, bool resize )
        {
            *const_cast<int *>( &properties.fullScreenRefreshRate )          = fullScreenRefreshRate;
            *const_cast<bool *>( &properties.resize )                        = resize;
            *const_cast<ENGINE_RESOLUTION_TYPE *>( &properties.size.width )  = width;
            *const_cast<ENGINE_RESOLUTION_TYPE *>( &properties.size.height ) = height;

            if ( properties.fullScreenRefreshRate )
            {
                glfwSetWindowMonitor( data->window, glfwGetPrimaryMonitor(), 0, 0, properties.size.width, properties.size.height, properties.fullScreenRefreshRate );
            }
            else
            {
                glfwSetWindowMonitor( data->window, 0, 0, 0, properties.size.width, properties.size.height, GLFW_DONT_CARE );
                glfwSetWindowAttrib( data->window, GLFW_RESIZABLE, properties.resize );
            }
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

        window::~window()
        {
            data->destroySurface( data->instance->data->handle );
        }

    } // namespace window
} // namespace Engine