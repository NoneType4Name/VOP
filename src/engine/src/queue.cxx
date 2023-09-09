#include <queue.hxx>
namespace Engine
{
    namespace tools
    {
        // queue::queue( void ) = default;

        queue::queue( VkDevice device, uint32_t familyIndex, uint32_t queueIndex ) : _familyIndex{ familyIndex }, _queueIndex{ queueIndex }
        {
            vkGetDeviceQueue( device, _familyIndex.value(), queueIndex, &_queue );
        }

        // queue::~queue( void ) = default;

        void queue::init( VkDevice device, uint32_t familyIndex, uint32_t queueIndex )
        {
            queue( device, familyIndex, queueIndex );
        }
        void queue::init( VkDevice device )
        {
            queue( device, _familyIndex.value(), _queueIndex );
        }

        void queue::setFamilyIndex( uint32_t index )
        {
            _familyIndex = index;
        }

        void queue::setQueueIndex( uint32_t index )
        {
            _queueIndex = index;
        }

        void queue::setHandle( VkQueue queue )
        {
            _queue = queue;
        }

        const VkQueue queue::GetHandle() const
        {
            return _queue;
        }

        const uint32_t queue::GetFamilyIndex() const
        {
            if( _familyIndex.has_value() )
                return _familyIndex.value();
            SPDLOG_CRITICAL( "Queue hasn't value." );
            return -1;
        }

        const uint32_t queue::GetQueueIndex() const
        {
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

        void Queues::operator=( std::initializer_list<uint32_t> right )
        {
            uint32_t i{ 0 };
            for( uint32_t val : right )
            {
                auto q{ ( ( tools::queue * )( ( &*this ) + sizeof( tools::queue ) * i++ ) ) };
                q->setFamilyIndex( val );
            }
        }

        tools::queue Queues::operator[]( size_t index )
        {
            return *reinterpret_cast<tools::queue *>( reinterpret_cast<char *>( this ) + sizeof( tools::queue ) * index );
        }

        std::unordered_map<uint32_t, std::pair<uint32_t, std::vector<float>>> &Queues::getUniqueIndeciesCount()
        {
            if( _unique.empty() )
                for( uint32_t i{ 0 }; i < count(); i++ )
                {
                    uint32_t index{ operator[]( i ).GetFamilyIndex() };
                    if( _unique.count( index ) )
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

        const size_t Queues::count() const
        {
            return ( ( sizeof( *this ) - sizeof( _unique ) ) / sizeof( tools::queue ) );
        }

        void Queues::init( VkDevice device )
        {
            for( uint32_t i{ 0 }; i < count(); i++ )
            {
                ( ( tools::queue * )( ( &*this ) + sizeof( tools::queue ) * i++ ) )->init( device );
            }
        }

        tools::Queues getIndecies( VkPhysicalDevice device )
        {
            tools::Queues _indecies;
            uint32_t _c{ 0 };
            vkGetPhysicalDeviceQueueFamilyProperties( device, &_c, nullptr );
            std::vector<VkQueueFamilyProperties> QueueFamilies( _c );
            vkGetPhysicalDeviceQueueFamilyProperties( device, &_c, QueueFamilies.data() );
            if( _c - 1 )
            {
                for( uint32_t i{ 0 }; i < QueueFamilies.size(); i++ )
                {

                    VkBool32 presentSupport{ false };
                    vkGetPhysicalDeviceSurfaceSupportKHR( device, i, tools::getSurface(), &presentSupport );
                    if( !_indecies.graphic.inited() && QueueFamilies[ i ].queueFlags & VK_QUEUE_GRAPHICS_BIT )
                    {
                        _indecies.graphic = i;
                    }
                    if( !_indecies.present.inited() && presentSupport )
                        _indecies.present = i;
                    else if( !_indecies.transfer.inited() && QueueFamilies[ i ].queueFlags & VK_QUEUE_TRANSFER_BIT )
                        _indecies.transfer = i;
                    // else if( !_indecies.compute.has_value() && QueueFamilies[ i ].queueFlags & VK_QUEUE_COMPUTE_BIT )
                    //     _indecies.compute = _c;
                    else
                        break;
                }
            }
            else
            {
                VkBool32 presentSupport{ false };
                vkGetPhysicalDeviceSurfaceSupportKHR( device, 0, tools::getSurface(), &presentSupport );
                if( QueueFamilies[ 0 ].queueFlags & ( VK_QUEUE_TRANSFER_BIT | VK_QUEUE_GRAPHICS_BIT ) && presentSupport )
                {
                    _indecies = { 0, 0, 0 };
                }
            }
            return _indecies;
        }

        VkDeviceQueueCreateInfo queueCreateInfo( uint32_t index, uint32_t count, const float *priority )
        {
            return { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, 0, 0, index, count, priority };
        }
    } // namespace tools
} // namespace Engine
