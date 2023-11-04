#include <EHI.hxx>
#include <queue.hxx>
namespace Engine
{
    namespace tools
    {
        queue::queue( VkDevice device, uint32_t familyIndex, uint32_t queueIndex )
        {
            init( device, familyIndex, queueIndex );
        }

        queue::queue( uint32_t familyIndex, uint32_t queueIndex )
        {
            _familyIndex = familyIndex;
            _queueIndex  = queueIndex;
        }

        void queue::init( VkDevice device )
        {
            init( device, _familyIndex.value(), _queueIndex );
        }

        void queue::init( VkDevice device, uint32_t familyIndex, uint32_t queueIndex )
        {
            _familyIndex = familyIndex;
            _queueIndex  = queueIndex;
            vkGetDeviceQueue( device, _familyIndex.value(), queueIndex, &_queue );
        }

        void queue::setFamilyIndex( uint32_t index )
        {
            _familyIndex = index;
        }

        void queue::setQueueIndex( uint32_t index )
        {
            _queueIndex = index;
        }

        // void queue::setHandle( VkQueue queue )
        // {
        //     _queue = queue;
        // }

        const VkQueue queue::getHandle() const
        {
            return _queue;
        }

        const uint32_t queue::getFamilyIndex() const
        {
            if ( !_familyIndex.has_value() )
            {
                SPDLOG_CRITICAL( "Queue hasn't value." );
                return -1;
            }
            return _familyIndex.value();
        }

        const uint32_t queue::getQueueIndex() const
        {
            if ( !_familyIndex.has_value() )
            {
                SPDLOG_CRITICAL( "Queue hasn't value." );
                return -1;
            }
            return _queueIndex;
        }

        const bool queue::inited() const
        {
            return _familyIndex.has_value();
        }

        void queue::operator=( uint32_t right )
        {
            setFamilyIndex( right );
        }

        void queue::operator=( std::array<uint32_t, 2> right )
        {
            setFamilyIndex( right[ 0 ] );
            setQueueIndex( right[ 1 ] );
        }

        void queueSet::operator=( std::initializer_list<uint32_t> right )
        {
            uint32_t i { 0 };
            for ( uint32_t val : right )
                ( *( tools::queue * ) ( ( &*this ) + sizeof( tools::queue ) * i++ ) ) = val;
        }

        tools::queue queueSet::operator[]( size_t index )
        {
            return *reinterpret_cast<tools::queue *>( reinterpret_cast<char *>( this ) + sizeof( tools::queue ) * index );
        }

        std::unordered_map<uint32_t, std::pair<uint32_t, std::vector<float>>> &queueSet::getUniqueIndecies()
        {
            if ( _unique.empty() )
                for ( uint32_t i { 0 }; i < count(); i++ )
                {
                    uint32_t index { operator[]( i ).getFamilyIndex() };
                    if ( _unique.count( index ) )
                    {
                        _unique[ index ].first += 1;
                    }
                    else
                    {
                        _unique[ index ].first = 1;
                    }
                    _unique[ index ].second.push_back( 1.f );
                }
            return _unique;
        }

        const size_t queueSet::count() const
        {
            return ( ( sizeof( *this ) - sizeof( _unique ) ) / sizeof( queue ) );
        }

        void queueSet::init( VkDevice device )
        {
            for ( uint32_t i { 0 }; i < count(); i++ )
            {
                ( ( queue * ) ( ( ( uint8_t * ) this ) + sizeof( queue ) * i ) )->init( device );
            }
        }
    } // namespace tools
} // namespace Engine
