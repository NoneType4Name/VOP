#include <ObjectiveVulkan/buffer.hxx>
#include <ObjectiveVulkan/device.hxx>
#include <ObjectiveVulkan/common/logging.hxx>

namespace Engine
{
    buffer::buffer( types::device device, VkBufferCreateInfo BufferCreateInfo, VkMemoryPropertyFlags memoryPropertiesFlag )
    {
        OBJECTIVE_VULKAN_OBJECTIVE_VULKAN_DEFINE_DATA_FIELD( device );
        data->device->data->buffers.insert( this );
        BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        OBJECTIVE_VULKAN_CHECK_RESULT( vkCreateBuffer( device->handle, &BufferCreateInfo, nullptr, &handle ) );
        data->addres = data->device->memory->allocate( handle, memoryPropertiesFlag );
    }

    buffer::~buffer()
    {
        data->device->memory->free( data->addres );
        vkDestroyBuffer( data->device->handle, handle, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
        data->device->data->buffers.erase( this );
    }

    buffer::OBJECTIVE_VULKAN_DATA_TYPE::OBJECTIVE_VULKAN_DATA_TYPE( types::buffer parent, types::device device ) :
        parent { parent }, device { device }
    {
    }

    buffer::OBJECTIVE_VULKAN_DATA_TYPE::~OBJECTIVE_VULKAN_DATA_TYPE()
    {
    }

    void buffer::write( const void *data, size_t size, uint32_t offset, VkMemoryMapFlags flags )
    {
        void *mapped;
        OBJECTIVE_VULKAN_CHECK_RESULT( vkMapMemory( this->data->device->handle, this->data->addres.memory, this->data->addres.offset + offset, size, flags, &mapped ) );
        memcpy( mapped, data, size );
        vkUnmapMemory( this->data->device->handle, this->data->addres.memory );
    }

    commandPool::commandPool( types::queue queue, VkCommandPoolCreateFlags flags )
    {
        OBJECTIVE_VULKAN_OBJECTIVE_VULKAN_DEFINE_DATA_FIELD( queue );
        data->queue->data->commandPools.insert( this );
        VkCommandPoolCreateInfo CommandPoolCreateInfo {};
        CommandPoolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        CommandPoolCreateInfo.flags            = flags;
        CommandPoolCreateInfo.queueFamilyIndex = data->queue->familyIndex;
        OBJECTIVE_VULKAN_CHECK_RESULT( vkCreateCommandPool( data->queue->data->device->handle, &CommandPoolCreateInfo, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK, &handle ) );
    }

    commandPool::~commandPool()
    {
        auto cmdB { data->commandBuffers.begin() };
        while ( cmdB != data->commandBuffers.end() )
            delete *cmdB++;
        vkDestroyCommandPool( data->queue->data->device->handle, handle, OBJECTIVE_VULKAN_ALLOCATION_CALLBACK );
        data->queue->data->commandPools.erase( this );
    }

    commandPool::OBJECTIVE_VULKAN_DATA_TYPE::OBJECTIVE_VULKAN_DATA_TYPE( types::commandPool parent, types::queue queue ) :
        parent { parent }, queue { queue }
    {
    }

    commandPool::OBJECTIVE_VULKAN_DATA_TYPE::~OBJECTIVE_VULKAN_DATA_TYPE()
    {
    }

    void commandPool::reset( VkCommandPoolResetFlags flags )
    {
        OBJECTIVE_VULKAN_CHECK_RESULT( vkResetCommandPool( data->queue->data->device->handle, handle, flags ) );
    }

    commandBuffer::commandBuffer( types::commandPool commandPool, VkCommandBufferLevel level )
    {
        OBJECTIVE_VULKAN_OBJECTIVE_VULKAN_DEFINE_DATA_FIELD( commandPool );
        data->pool->data->commandBuffers.insert( this );
        VkCommandBufferAllocateInfo CommandBufferAllocateInfo {};
        CommandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        CommandBufferAllocateInfo.commandPool        = data->pool->handle;
        CommandBufferAllocateInfo.level              = level;
        CommandBufferAllocateInfo.commandBufferCount = 1;
        OBJECTIVE_VULKAN_CHECK_RESULT( vkAllocateCommandBuffers( data->pool->data->queue->data->device->handle, &CommandBufferAllocateInfo, &handle ) );
    }

    commandBuffer::~commandBuffer()
    {
        if ( data->used )
            end();
        vkFreeCommandBuffers( data->pool->data->queue->data->device->handle, data->pool->handle, 1, &handle );
        data->pool->data->commandBuffers.erase( this );
    }

    void commandBuffer::begin()
    {
        if ( data->used )
        {
            SPDLOG_CRITICAL( "Allready begined command buffer." );
            return;
        };
        data->used = 1;
        VkCommandBufferBeginInfo CommandBufferBeginInfo {};
        CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        OBJECTIVE_VULKAN_CHECK_RESULT( vkBeginCommandBuffer( handle, &CommandBufferBeginInfo ) );
    }

    void commandBuffer::end()
    {
        if ( !data->used )
        {
            return;
            SPDLOG_CRITICAL( "Not started." );
        }
        data->used = 0;
        OBJECTIVE_VULKAN_CHECK_RESULT( vkEndCommandBuffer( handle ) );
    }

    void commandBuffer::submit( VkFence fence )
    {
        if ( data->used )
            end();

        VkSubmitInfo SubmitInfo {};
        SubmitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers    = &handle;
        OBJECTIVE_VULKAN_CHECK_RESULT( vkResetFences( data->pool->data->queue->data->device->handle, 1, &fence ) );
        OBJECTIVE_VULKAN_CHECK_RESULT( vkQueueSubmit( data->pool->data->queue->handle, 1, &SubmitInfo, fence ) );
        OBJECTIVE_VULKAN_CHECK_RESULT( vkWaitForFences( data->pool->data->queue->data->device->handle, 1, &fence, true, -1ui64 ) );
    }

    void commandBuffer::submit()
    {
        if ( data->used )
            end();

        VkSubmitInfo SubmitInfo {};
        SubmitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers    = &handle;
        OBJECTIVE_VULKAN_CHECK_RESULT( vkQueueSubmit( data->pool->data->queue->handle, 1, &SubmitInfo, nullptr ) );
    }

    VkResult commandBuffer::reset( VkCommandBufferResetFlags flags )
    {
        return vkResetCommandBuffer( handle, flags );
    }

    commandBuffer::OBJECTIVE_VULKAN_DATA_TYPE::OBJECTIVE_VULKAN_DATA_TYPE( types::commandBuffer parent, types::commandPool commandPool ) :
        pool { commandPool }
    {
    }

    commandBuffer::OBJECTIVE_VULKAN_DATA_TYPE::~OBJECTIVE_VULKAN_DATA_TYPE()
    {
    }
} // namespace Engine