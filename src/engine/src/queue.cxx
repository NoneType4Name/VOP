#include <queue.hxx>
#include <device.hxx>
#include <instance.hxx>
#include <common/logging.hxx>

namespace Engine
{
    queue::queue( types::device device, uint32_t familyIndex, uint32_t queueIndex, float priority ) :
        index( queueIndex ), familyIndex { familyIndex }, priority { priority }
    {
        OBJECTIVE_VULKAN_OBJECTIVE_VULKAN_DEFINE_DATA_FIELD( device );
        data->device->data->queues.insert( this );
        vkGetDeviceQueue( device->handle, this->familyIndex, index, &handle );
    }

    queue::~queue()
    {
        OBJECTIVE_VULKAN_CHECK_RESULT( vkQueueWaitIdle( handle ) );
        auto cmdP { data->commandPools.begin() };
        while ( cmdP != data->commandPools.end() )
            delete *cmdP++;
        data->device->data->queues.erase( this );
    }

    queue::OBJECTIVE_VULKAN_DATA_TYPE::OBJECTIVE_VULKAN_DATA_TYPE( types::queue parent, types::device device ) :
        parent { parent }, device { device }
    {
    }

    queue::OBJECTIVE_VULKAN_DATA_TYPE::~OBJECTIVE_VULKAN_DATA_TYPE()
    {
    }

    bool queue::operator==( const queue &right ) const noexcept
    {
        if ( this->familyIndex == right.familyIndex && this->index == right.index && this->priority == right.priority )
            return true;
        return false;
    }
} // namespace Engine
