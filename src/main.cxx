#ifdef _DEBUG
#    define DEBUG               true
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
#    define DEBUG               false
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_CRITICAL
#endif

#include <engine.hxx>
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
    Engine::types::instance engine { new Engine::instance { "test", 0 } };
    Engine::window::types::window window { engine->createWindow( 120, 120, "window#1" ) };
    const std::vector<Engine::types::DeviceDescription> devices = engine->GetDevices();
    Engine::types::link link { engine->CreateLink( window, devices.back() ) };
} // namespace Game

int main()
{
    while ( !Game::window->shouldClose() )
    {
        Game::window->updateEvents();
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
