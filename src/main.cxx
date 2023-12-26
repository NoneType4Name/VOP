#ifdef _DEBUG
#    define DEBUG               true
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
#    define DEBUG               false
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_CRITICAL
#endif

// #include <engine_setup.hxx>
#include <engine.hxx>
#include <EHI.hxx>
#include <surface.hxx>
#include <spdlog/spdlog.h>
#include <iostream>

namespace
{
    struct _
    {
        _()
        {
            try
            {
                spdlog::set_level( DEBUG ? spdlog::level::trace : spdlog::level::critical );
                spdlog::set_pattern( "[%H:%M:%S.%e] [%^%l%$] %v" );
                SPDLOG_DEBUG( "--- Start logging ---" );
            }
            catch ( const std::exception &logInitE )
            {
                std::cerr << "Logger initialize error:\t" << logInitE.what() << "\n.";
                exit( EXIT_FAILURE );
            }
        }
        ~_()
        {
            SPDLOG_DEBUG( "--- Logging end ---" );
            spdlog::shutdown();
        }
    } _;
} // namespace

namespace Game
{
    class E;

    struct W : public Engine::window::window
    {
      protected:
        void setup() override
        {
            data->createSurface( data->instance->data->handle, 0, 0 );
        }

        void eventCallBack( int key, int scancode, int action, int mods ) override
        {
            switch ( key )
            {
                case GLFW_KEY_F11:
                    auto d { getDisplayResolution() };
                    if ( action == GLFW_RELEASE )
                    {
                        if ( properties.size.width != getDisplayResolution().width )
                        {
                            updateProperties( { .size = d, .fullScreenRefreshRate = 60 } );
                            break;
                        }
                        updateProperties( { .size = { .width = 800, .height = 600 }, .fullScreenRefreshRate = 0 } );
                    }
            }
        }

        void resizeCallBack( int width, int height ) override
        {
        }
        friend E;

      public:
        using Engine::window::window::window;
    };

    struct E : public Engine::instance
    {
        Engine::window::types::window createWindow( ENGINE_RESOLUTION_TYPE width, ENGINE_RESOLUTION_TYPE height, const char *title, int fullScreenRefreshRate, bool resize ) override
        {
            return data->regWindow( new W { this, { width, height, title, fullScreenRefreshRate, resize } } );
        }

      protected:
        void setup( const char *appName, uint32_t appVersion ) override
        {
            VkApplicationInfo ApplicationInfo {};
            ApplicationInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            ApplicationInfo.apiVersion         = VK_API_VERSION_1_0;
            ApplicationInfo.pApplicationName   = appName;
            ApplicationInfo.applicationVersion = appVersion;
            VkInstanceCreateInfo InstanceCreateInfo {};
            InstanceCreateInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            InstanceCreateInfo.pNext                   = nullptr;
            InstanceCreateInfo.enabledLayerCount       = 0;
            InstanceCreateInfo.ppEnabledLayerNames     = nullptr;
            InstanceCreateInfo.enabledExtensionCount   = 0;
            InstanceCreateInfo.ppEnabledExtensionNames = nullptr;
            InstanceCreateInfo.pApplicationInfo        = &ApplicationInfo;
            data->create( InstanceCreateInfo );
        }
    };
    std::unique_ptr<E> engine { new E };

    // Engine::window::types::window window { engine->createWindow( 100, 100, "window#1" ) };
    // const std::vector<Engine::types::DeviceDescription> devices = engine->GetDevices();
    // auto [ link, device ] { engine->CreateLink( window, devices.back() ) };
    // Engine::types::shader vertexShader { device->CreateShader( "./assets/shaders/binary.vert.spv", "main", Engine::ShaderStage::VERTEX_SHADER_TYPE ) };
    // Engine::types::shader fragmentShader { device->CreateShader( "./assets/shaders/binary.frag.spv", "main", Engine::ShaderStage::FRAGMENT_SHADER_TYPE ) };
    // Engine::types::pass pass { engine->CreateRenderPass( link ) };
    // Engine::types::pipeline pipeline { device->CreatePipeline( layout, { vertexShader, fragmentShader }, pass ) };
} // namespace Game

int main()
{
    Game::engine->init( "test", 0 );
    auto wnd { Game::engine->createWindow( 800, 600, "test", 0, 1 ) };
    while ( !wnd->shouldClose() )
    {
        wnd->updateEvents();
    }
    // Engine::types::device device { engine->CreateDevice( devices[ 0 ] ) };
    // Engine::types::Device
    // engine.InitDevice( devices[ 0 ] );
    // auto rectangle{ Engine::CreateModel( Engine::modelType::MODEL_TYPE_ENTITY, "./assets/models/rectangle/model.obj" ) };
    // auto rectangle_texture { Engine::CreateTexture( "./assets/textures/rectangle/model.png" ) };
    // auto rectangle_model { Engine::CreateModel( Engine::modelType::MODEL_TYPE_ENTITY, "./assets/models/rectangle/model.obj" ) };
    // auto devices { Engine::GetGraphicDevices( Engine::DISCRETE_GPU | Engine::INTEGRATED_GPU ) };
    // auto pipeline { Engine::CreatePipeline(
    //     { { Engine::CreateShader( "./assets/shaders/binary.vert.spv", "main", Engine::ShaderStage::VERTEX_SHADER_TYPE ),
    //         Engine::CreateShader( "./assets/shaders/binary.frag.spv", "main", Engine::ShaderStage::FRAGMENT_SHADER_TYPE ) } } ) };
    // Engine::AppCreateInfo App {};
    // App.width                       = 0;
    // App.height                      = 0;
    // App.title                       = "Quest App.";
    // App.device                      = devices.front();
    // App.settings.MultiSamplingCount = Engine::MultiSamplingCount::SAMPLE_COUNT_2_BIT;

    // Engine::init( App );
    // Engine::window::setKeyEventsCallback( []( int key, int scancode, int action, int mods )
    //                                       { SPDLOG_DEBUG( "key pressed code: {}", std::to_string( key ) ); } );
    // while ( !Engine::window::shouldClose() )
    // {
    //     Engine::window::updateEvents();
    // }
    // Engine::shutdown();
    return 0;
}
