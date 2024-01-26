#include <queue.hxx>
#include <device.hxx>
#include <instance.hxx>

namespace Engine
{
    queue::queue( types::device device, uint32_t familyIndex, uint32_t queueIndex, float priority ) :
        index( queueIndex ), familyIndex { familyIndex }, priority { priority }
    {
        DEFINE_DATA_FIELD( device );
        vkGetDeviceQueue( device->handle, this->familyIndex, index, &handle );
    }

    queue::~queue()
    {
        vkQueueWaitIdle( handle );
        for ( const auto &cmdP : data->commandPools )
            delete cmdP;
    }

    queue::DATA_TYPE::DATA_TYPE( types::queue parent, types::device device ) :
        parent { parent }, device { device }
    {
    }

    queue::DATA_TYPE::~DATA_TYPE()
    {
    }

    // void queue::init( VkDevice device )
    // {
    //     init( device, familyIndex.value(), index, priority );
    // }

    // void queue::init( VkDevice device, uint32_t familyIndex, uint32_t queueIndex, float priority )
    // {
    // }

    // void queue::operator=( std::pair<uint32_t, float> right )
    // {
    //     familyIndex = right.first;
    //     priority    = right.second;
    //     if ( set->parent->data->description->data->queueFamilyProperties[ 0 ].queueCount > 1 )
    //         index = set->getUniqueIndecies()[ right.first ].prioreties.size() - 1;
    // }

    // void queue::operator=( std::tuple<uint32_t, uint32_t, float> right )
    // {
    //     familyIndex = std::get<0>( right );
    //     index       = std::get<1>( right );
    //     priority    = std::get<2>( right );
    // }

    bool queue::operator==( const queue &right )
    {
        if ( this->familyIndex == right.familyIndex && this->index == right.index && this->priority == right.priority )
            return true;
        return false;
    }

    // bool queue::operator!=( const queue &right )
    // {
    //     return !operator==( right );
    // }

    // queueSet::queueSet() :
    //     graphic { this }, present { this }, transfer { this } {}

    // queueSet::queueSet( types::device parent ) :
    //     queueSet {}
    // {
    //     this->parent = parent;
    // }

    // queueSet::~queueSet() {}

    // void queueSet::operator=( std::initializer_list<std::pair<uint32_t, float>> right )
    // {
    //     uint32_t i { 0 };
    //     for ( auto val : right )
    //         *operator[]( i++ ) = val;
    // }

    // void queueSet::operator=( std::initializer_list<std::tuple<uint32_t, uint32_t, float>> right )
    // {
    //     uint32_t i { 0 };
    //     for ( auto val : right )
    //         *operator[]( i++ ) = val;
    // }

    // queue *queueSet::operator[]( size_t index )
    // {
    //     auto d { &graphic };
    //     auto i { ( ( reinterpret_cast<queue *>( &graphic ) ) + index ) };
    //     return ( ( reinterpret_cast<queue *>( &graphic ) ) + index );
    // }
    // // <family<index in family, ptr to priority>, vector of all prioreties in family>
    // queuesProperties &queueSet::getUniqueIndecies()
    // {
    //     _unique.clear();
    //     for ( uint32_t i { 0 }; i < count(); i++ )
    //     {
    //         if ( !operator[]( i )->familyIndex.has_value() ||
    //              ( _unique[ operator[]( i )->familyIndex.value() ].indeciesPriorety.count( operator[]( i )->index ) ) )
    //             continue;
    //         _unique[ operator[]( i )->familyIndex.value() ].flags = operator[]( i )->flags;
    //         _unique[ operator[]( i )->familyIndex.value() ].prioreties.push_back( operator[]( i )->priority );
    //         _unique[ operator[]( i )->familyIndex.value() ].indeciesPriorety[ operator[]( i )->index ] = &_unique[ operator[]( i )->familyIndex.value() ].prioreties.back();
    //     }
    //     return _unique;
    // }

    // const size_t queueSet::count() const
    // {
    //     return ( ( sizeof( *this ) - sizeof( _unique ) - sizeof( void * ) ) / sizeof( queue ) );
    // }

    // void queueSet::init( VkDevice device )
    // {
    //     for ( uint32_t i { 0 }; i < count(); i++ )
    //     {
    //         if ( operator[]( i )->familyIndex.has_value() )
    //             operator[]( i )->init( device );
    //     }
    // }
} // namespace Engine
