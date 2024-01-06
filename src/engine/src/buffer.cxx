#include <buffer.hxx>

namespace Engine
{
    buffer::buffer( types::device device ) :
        device { device }
    {
    }

    buffer::buffer( types::device device, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertiesFlag, VkDeviceSize size, VkSharingMode sharingMode, VkBufferCreateFlags flags, void *pNext ) :
        buffer( device )
    {
        init( usageFlags, memoryPropertiesFlag, size, sharingMode, flags, pNext );
    }

    buffer::~buffer()
    {
        // device->data->resetBufferMemory( handle, mIndex );
        vkDestroyBuffer( device->data->handle, handle, ALLOCATION_CALLBACK );
    }

    void buffer::init( VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertiesFlag, VkDeviceSize size, VkSharingMode sharingMode, VkBufferCreateFlags flags, void *pNext )
    {
        VkBufferCreateInfo BufferCreateInfo {};
        BufferCreateInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufferCreateInfo.flags       = flags;
        BufferCreateInfo.pNext       = pNext;
        BufferCreateInfo.size        = size;
        BufferCreateInfo.usage       = usageFlags;
        BufferCreateInfo.sharingMode = sharingMode;

        CHECK_RESULT( vkCreateBuffer( device->data->handle, &BufferCreateInfo, nullptr, &handle ) );
        this->device->data->allocateBufferMemory( handle, memoryPropertiesFlag );
    }

    void buffer::copy( void *data, VkDeviceSize size, VkMemoryMapFlags flags )
    {
        device->data->writeBufferMemory( handle, flags, &data, size );
    }

    commandBuffer::commandBuffer( types::device device, VkCommandPool commandPool, VkCommandBufferLevel level, Engine::queue *queue )
    {
        this->device = device;
        this->queue  = queue;
        VkCommandBufferAllocateInfo CommandBufferAllocateInfo {};
        CommandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        CommandBufferAllocateInfo.commandPool        = commandPool; // todo
        CommandBufferAllocateInfo.level              = level;
        CommandBufferAllocateInfo.commandBufferCount = 1; // todo
        CHECK_RESULT( vkAllocateCommandBuffers( device->data->handle, &CommandBufferAllocateInfo, &handle ) );

        VkFenceCreateInfo FenceCreateInfo;
        FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        CHECK_RESULT( vkCreateFence( device->data->handle, &FenceCreateInfo, nullptr, &fence ) );
    }

    commandBuffer::~commandBuffer()
    {
        vkFreeCommandBuffers( device->data->handle, pool, 1, &handle );
        vkDestroyFence( device->data->handle, fence, ALLOCATION_CALLBACK );
    }

    void commandBuffer::begin()
    {
        if ( handle == nullptr )
            SPDLOG_CRITICAL( "Failed to begin command buffer." );
        if ( began ) return;
        VkCommandBufferBeginInfo CommandBufferBeginInfo {};
        CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer( handle, &CommandBufferBeginInfo );
    }

    void commandBuffer::submit()
    {
        end();
        VkSubmitInfo SubmitInfo {};
        SubmitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers    = &handle;
        vkResetFences( device->data->handle, 1, &fence );
        vkQueueSubmit( queue->handle, 1, &SubmitInfo, fence );
        vkWaitForFences( device->data->handle, 1, &fence, true, -1ui64 );
    }

    void commandBuffer::end()
    {
        if ( handle == nullptr )
            SPDLOG_CRITICAL( "Failed to end command buffer." );
        if ( used )
            vkEndCommandBuffer( handle );
    }
} // namespace Engine