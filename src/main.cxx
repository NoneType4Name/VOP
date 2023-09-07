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
    Engine::AppCreateInfo App{};
    App.width              = 0;
    App.height             = 0;
    App.title              = "Quest App.";
    App.VertexShaderPath   = "./assets/shaders/binary.vert.spv";
    App.FragmentShaderPath = "./assets/shaders/binary.frag.spv";
    // App.vAppModels.push_back( { "./assets/models/rectangle/model.obj", "./assets/textures/rectangle/model.png" } );
    // App.vAppModels.push_back( { "./assets/models/rectangle/model.obj", "./assets/textures/rectangle/model2.png" } );
    App.sSettings.MultiSamplingCount = 2;
    Engine::init( App );
    Engine::shutdown();
    // Engine::SetKeyEventsCallback( []( int key, int scancode, int action, int mods )
    //                               { SPDLOG_DEBUG( "key pressed code: {}", std::to_string( key ) ); } );
    // while( !Engine::WindowShouldClose() )
    // {
    //     Engine::UpdateEvents();
    //     // auto d = Engine::GrapchicPhysicalDevice{};
    // }
    return 0;
}
