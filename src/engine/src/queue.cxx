#include <queue.hxx>
#include <EHI.hxx>

namespace Engine
{
    void InstanceSetup::queueFlags( queueSet *queueSet, VkDeviceQueueCreateFlags &flags, void *userPoiner ) {}
    queue::queue( queueSet *pSet )
    {
        set = pSet;
    }

    queue::queue( queueSet *pSet, VkDevice device, uint32_t familyIndex, uint32_t queueIndex, float priority )
    {
        set            = pSet;
        this->priority = priority;
        init( device, familyIndex, queueIndex );
    }

    queue::queue( queueSet *pSet, uint32_t familyIndex, uint32_t queueIndex, float priority )
    {
        set               = pSet;
        this->familyIndex = familyIndex;
        index             = queueIndex;
        this->priority    = priority;
    }

    void queue::init( VkDevice device )
    {
        init( device, familyIndex.value(), index, priority );
    }

    void queue::init( VkDevice device, uint32_t familyIndex, uint32_t queueIndex, float priority )
    {
        this->familyIndex = familyIndex;
        index             = queueIndex;
        this->priority    = priority;
        vkGetDeviceQueue( device, this->familyIndex.value(), queueIndex, &handle );
    }

    void queue::operator=( std::pair<uint32_t, float> right )
    {
        familyIndex = right.first;
        priority    = right.second;
        if ( set->description->data->queueFamilyProperties[ 0 ].queueCount > 1 )
            index = set->getUniqueIndecies()[ right.first ].second.size() - 1;
    }

    void queue::operator=( std::tuple<uint32_t, uint32_t, float> right )
    {
        familyIndex = std::get<0>( right );
        index       = std::get<1>( right );
        priority    = std::get<2>( right );
    }

    bool queue::operator==( const queue &right )
    {
        if ( this->familyIndex.has_value() || right.familyIndex.has_value() )
            if ( this->familyIndex.value() == right.familyIndex.value() && this->index == right.index && this->priority == right.priority )
                return true;
        return false;
    }

    bool queue::operator!=( const queue &right )
    {
        return !operator==( right );
    }

    queueSet::queueSet() :
        graphic { this }, present { this }, transfer { this } {}

    queueSet::queueSet( DeviceDescription *description ) :
        queueSet {}
    {
        this->description = description;
    }

    void queueSet::operator=( std::initializer_list<std::pair<uint32_t, float>> right )
    {
        uint32_t i { 0 };
        for ( auto val : right )
            *operator[]( i++ ) = val;
    }

    void queueSet::operator=( std::initializer_list<std::tuple<uint32_t, uint32_t, float>> right )
    {
        uint32_t i { 0 };
        for ( auto val : right )
            *operator[]( i++ ) = val;
    }

    queue *queueSet::operator[]( size_t index )
    {
        auto d { &graphic };
        auto i { ( ( reinterpret_cast<queue *>( &graphic ) ) + index ) };
        return ( ( reinterpret_cast<queue *>( &graphic ) ) + index );
    }
    // <family<index in family, ptr to priority>, vector of all prioreties in family>
    std::unordered_map<uint32_t, std::pair<std::unordered_map<uint32_t, float *>, std::vector<float>>> &queueSet::getUniqueIndecies()
    {
        _unique.clear();
        for ( uint32_t i { 0 }; i < count(); i++ )
        {
            if ( !operator[]( i )->familyIndex.has_value() ||
                 ( _unique[ operator[]( i )->familyIndex.value() ].first.count( operator[]( i )->index ) ) )
                continue;
            _unique[ operator[]( i )->familyIndex.value() ].second.push_back( operator[]( i )->priority );
            _unique[ operator[]( i )->familyIndex.value() ].first[ operator[]( i )->index ] = &_unique[ operator[]( i )->familyIndex.value() ].second.back();
        }
        return _unique;
    }

    const size_t queueSet::count() const
    {
        return ( ( sizeof( *this ) - sizeof( _unique ) - sizeof( void * ) ) / sizeof( queue ) );
    }

    void queueSet::init( VkDevice device )
    {
        for ( uint32_t i { 0 }; i < count(); i++ )
        {
            operator[]( i )->init( device );
        }
    }
} // namespace Engine
