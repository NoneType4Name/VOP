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
        }
        ~_()
        {
            Engine::WindowDestroy();
            spdlog::shutdown();
        }
    } _;
} // namespace

int main()
{
    while( 1 )
    {
    }
    return 0;
}