#include <buffer.hxx>
#include <device.hxx>

namespace Engine
{
    buffer::buffer( types::device device ) :
        device { device }
    {
    }

    buffer::~buffer()
    {
        if ( mapped != nullptr )
            unmap();
        vkFreeMemory( device->data->device, memory, ALLOCATION_CALLBACK );
        vkDestroyBuffer( device->data->device, handle, ALLOCATION_CALLBACK );
    }

    void buffer::init( VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertiesFlag, VkDeviceSize size, VkSharingMode sharingMode, VkBufferCreateFlags flags, void *pNext, void *pNextAllocate )
    {
        VkBufferCreateInfo BufferCreateInfo {};
        BufferCreateInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        BufferCreateInfo.flags       = flags;
        BufferCreateInfo.pNext       = pNext;
        BufferCreateInfo.size        = size;
        BufferCreateInfo.usage       = usageFlags;
        BufferCreateInfo.sharingMode = sharingMode;

        CHECK_RESULT( vkCreateBuffer( device->data->device, &BufferCreateInfo, nullptr, &handle ) );
        VkMemoryRequirements Requirements;
        vkGetBufferMemoryRequirements( device->data->device, handle, &Requirements );
        VkMemoryAllocateInfo MemoryAllocateInfo {};
        MemoryAllocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        MemoryAllocateInfo.pNext           = pNextAllocate;
        MemoryAllocateInfo.allocationSize  = Requirements.size;
        MemoryAllocateInfo.memoryTypeIndex = tools::requeredMemoryTypeIndex( device, Requirements.memoryTypeBits, memoryPropertiesFlag );
        CHECK_RESULT( vkAllocateMemory( device->data->device, &MemoryAllocateInfo, ALLOCATION_CALLBACK, &memory ) );
        CHECK_RESULT( vkBindBufferMemory( device->data->device, handle, memory, 0 ) ); // allocate todo?
    }

    VkResult buffer::map( VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags )
    {
        if ( memory == nullptr )
            SPDLOG_CRITICAL( "Failed to map memory." );
        if ( mapped != nullptr )
            return VK_SUCCESS;
        return vkMapMemory( device->data->device, memory, offset, size, flags, &mapped );
    }

    void buffer::copy( void *data, VkDeviceSize size )
    {
        if ( memory == nullptr )
            SPDLOG_CRITICAL( "Failed to copy memory." );
        if ( mapped == nullptr )
            return;
        memcpy( mapped, data, size );
    }

    void buffer::unmap()
    {
        if ( memory == nullptr )
            SPDLOG_CRITICAL( "Failed to unmap memory." );
        if ( mapped == nullptr )
            return;
        vkUnmapMemory( device->data->device, memory );
        mapped = nullptr;
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
        CHECK_RESULT( vkAllocateCommandBuffers( device->data->device, &CommandBufferAllocateInfo, &handle ) );

        VkFenceCreateInfo FenceCreateInfo;
        FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        CHECK_RESULT( vkCreateFence( device->data->device, &FenceCreateInfo, nullptr, &fence ) );
    }

    commandBuffer::~commandBuffer()
    {
        vkFreeCommandBuffers( device->data->device, pool, 1, &handle );
        vkDestroyFence( device->data->device, fence, ALLOCATION_CALLBACK );
    }

    VkCommandBuffer commandBuffer::getHandle() const
    {
        return handle;
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
        vkResetFences( device->data->device, 1, &fence );
        vkQueueSubmit( queue->handle, 1, &SubmitInfo, fence );
        vkWaitForFences( device->data->device, 1, &fence, true, -1ui64 );
    }

    void commandBuffer::end()
    {
        if ( handle == nullptr )
            SPDLOG_CRITICAL( "Failed to end command buffer." );
        if ( began )
            vkEndCommandBuffer( handle );
    }
} // namespace Engine