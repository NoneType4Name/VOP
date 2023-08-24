#include <engine.hxx>

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
            Engine::WindowInit( 0, 0, "Quest App." );
            Engine::SetKeyEventsCallback( []( int key, int scancode, int action, int mods )
                                          { SPDLOG_DEBUG( "key pressed code: {}", std::to_string( key ) ); } );
        }
        ~_()
        {
            Engine::WindowDestroy();
            SPDLOG_DEBUG( "--- Logging end ---" );
            spdlog::shutdown();
        }
    } _;
} // namespace

int main()
{
    while( !Engine::WindowShouldClose() )
    {
        Engine::UpdateEvents();
    }
    return 0;
}
