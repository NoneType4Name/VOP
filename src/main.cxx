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
            catch( const std::exception &logInitE )
            {
                std::cerr << "Logger initialize error:\t" << logInitE.what() << "\n.";
                exit( EXIT_FAILURE );
            }
        }
        ~_()
        {
            // Engine::shutdown();
            SPDLOG_DEBUG( "--- Logging end ---" );
            spdlog::shutdown();
        }
    } _;
} // namespace

int main()
{
    // auto rectangle{ Engine::CreateModel( Engine::modelType::MODEL_TYPE_ENTITY, "./assets/models/rectangle/model.obj" ) };
    auto rectangle_texture{ Engine::CreateTexture( "./assets/textures/rectangle/model.png" ) };
    auto rectangle_model{ Engine::CreateModel( Engine::modelType::MODEL_TYPE_ENTITY, "./assets/models/rectangle/model.obj" ) };
    auto devices{ Engine::GetGraphicDevices( Engine::DISCRETE_GPU | Engine::INTEGRATED_GPU ) };
    Engine::AppCreateInfo App{};
    App.width                       = 0;
    App.height                      = 0;
    App.title                       = "Quest App.";
    App.device                      = devices.front();
    App.VertexShaderPath            = "./assets/shaders/binary.vert.spv";
    App.FragmentShaderPath          = "./assets/shaders/binary.frag.spv";
    App.settings.MultiSamplingCount = Engine::MultiSamplingCount::SAMPLE_COUNT_2_BIT;

    Engine::init( App );
    // Engine::window::setKeyEventsCallback( []( int key, int scancode, int action, int mods )
    //   { SPDLOG_DEBUG( "key pressed code: {}", std::to_string( key ) ); } );
    while( !Engine::window::shouldClose() )
    {
        Engine::window::updateEvents();
    }
    Engine::shutdown();
    return 0;
}
