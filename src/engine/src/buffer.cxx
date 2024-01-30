#include <buffer.hxx>
#include <device.hxx>

namespace Engine
{
    buffer::buffer( types::device device, VkBufferCreateInfo BufferCreateInfo, VkMemoryPropertyFlags memoryPropertiesFlag )
    {
        DEFINE_DATA_FIELD( device );
        data->device->data->buffers.insert( this );
        BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        CHECK_RESULT( vkCreateBuffer( device->handle, &BufferCreateInfo, nullptr, &handle ) );
        data->addres = data->device->memory.allocate( handle, memoryPropertiesFlag );
    }

    buffer::~buffer()
    {
        data->device->memory.free( data->addres );
        vkDestroyBuffer( data->device->handle, handle, ALLOCATION_CALLBACK );
        data->device->data->buffers.erase( this );
    }

    buffer::DATA_TYPE::DATA_TYPE( types::buffer parent, types::device device ) :
        parent { parent }, device { device }
    {
    }

    buffer::DATA_TYPE::~DATA_TYPE()
    {
    }

    void buffer::write( types::commandBuffer commandBuffer, std::vector<void *> &data, VkMemoryMapFlags flags )
    {
        VkBuffer TransferBuffer;
        std::vector<void *> mapped( data.size() );
        VkBufferCreateInfo bCI { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bCI.size                  = data.size();
        bCI.queueFamilyIndexCount = 1;
        bCI.pQueueFamilyIndices   = &commandBuffer->data->queue->familyIndex;
        bCI.usage                 = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bCI.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
        CHECK_RESULT( vkCreateBuffer( this->data->device->handle, &bCI, ALLOCATION_CALLBACK, &TransferBuffer ) );
        auto TransferBufferMemoryAddr { this->data->device->memory.allocate( TransferBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ) };
        CHECK_RESULT( vkMapMemory( this->data->device->handle, TransferBufferMemoryAddr.memory, TransferBufferMemoryAddr.offset, mapped.size(), flags, mapped.data() ) );
        memcpy( mapped.data(), data.data(), data.size() );
        vkUnmapMemory( this->data->device->handle, TransferBufferMemoryAddr.memory );
        VkBufferCopy copyInfo {};
        copyInfo.srcOffset = TransferBufferMemoryAddr.offset;
        copyInfo.dstOffset = this->data->addres.offset;
        copyInfo.size      = bCI.size;
        if ( !commandBuffer->data->used )
            commandBuffer->begin();
        vkCmdCopyBuffer( commandBuffer->handle, TransferBuffer, this->handle, 1, &copyInfo );
    }

    void buffer::write( types::commandPool commandPool, types::queue queue, std::vector<void *> &data, VkMemoryMapFlags flags )
    {
        auto cmdB { new commandBuffer { commandPool, queue, VK_COMMAND_BUFFER_LEVEL_PRIMARY } };
        VkFence fence;
        VkFenceCreateInfo fCI { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        vkCreateFence( commandPool->data->queue->data->device->handle, &fCI, ALLOCATION_CALLBACK, &fence );
        VkSubmitInfo submit {};
        submit.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.commandBufferCount = 1;
        submit.pCommandBuffers    = &cmdB->handle;
        CHECK_RESULT( vkQueueSubmit( queue->handle, 1, &submit, fence ) );
        CHECK_RESULT( vkWaitForFences( queue->data->device->handle, 1, &fence, true, -1ui64 ) );
        delete cmdB;
    }

    commandPool::commandPool( types::queue queue, VkCommandPoolCreateFlags flags )
    {
        DEFINE_DATA_FIELD( queue );
        data->queue->data->commandPools.insert( this );
        VkCommandPoolCreateInfo CommandPoolCreateInfo {};
        CommandPoolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        CommandPoolCreateInfo.flags            = flags;
        CommandPoolCreateInfo.queueFamilyIndex = data->queue->familyIndex;
        CHECK_RESULT( vkCreateCommandPool( data->queue->data->device->handle, &CommandPoolCreateInfo, ALLOCATION_CALLBACK, &handle ) );
    }

    commandPool::~commandPool()
    {
        for ( const auto &cmdB : data->commandBuffers )
            delete cmdB;
        CHECK_RESULT( vkResetCommandPool( data->queue->data->device->handle, handle, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT ) );
        data->queue->data->commandPools.erase( this );
    }

    commandPool::DATA_TYPE::DATA_TYPE( types::commandPool parent, types::queue queue ) :
        parent { parent }, queue { queue }
    {
    }

    commandPool::DATA_TYPE::~DATA_TYPE()
    {
    }

    void commandPool::reset( VkCommandPoolResetFlags flags )
    {
        CHECK_RESULT( vkResetCommandPool( data->queue->data->device->handle, handle, flags ) );
    }

    commandBuffer::commandBuffer( types::commandPool commandPool, types::queue queue, VkCommandBufferLevel level )
    {
        DEFINE_DATA_FIELD( commandPool, queue );
        data->pool->data->commandBuffers.insert( this );
        VkCommandBufferAllocateInfo CommandBufferAllocateInfo {};
        CommandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        CommandBufferAllocateInfo.commandPool        = data->pool->handle;
        CommandBufferAllocateInfo.level              = level;
        CommandBufferAllocateInfo.commandBufferCount = 1;
        CHECK_RESULT( vkAllocateCommandBuffers( data->pool->data->queue->data->device->handle, &CommandBufferAllocateInfo, &handle ) );
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
        CHECK_RESULT( vkBeginCommandBuffer( handle, &CommandBufferBeginInfo ) );
    }

    void commandBuffer::end()
    {
        if ( !data->used )
        {
            return;
            SPDLOG_CRITICAL( "Not started." );
        }
        data->used = 0;
        CHECK_RESULT( vkEndCommandBuffer( handle ) );
    }

    void commandBuffer::submit( VkFence fence )
    {
        if ( data->used )
            end();

        VkSubmitInfo SubmitInfo {};
        SubmitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers    = &handle;
        CHECK_RESULT( vkResetFences( data->pool->data->queue->data->device->handle, 1, &fence ) );
        CHECK_RESULT( vkQueueSubmit( data->queue->handle, 1, &SubmitInfo, fence ) );
        CHECK_RESULT( vkWaitForFences( data->pool->data->queue->data->device->handle, 1, &fence, true, -1ui64 ) );
    }

    commandBuffer::DATA_TYPE::DATA_TYPE( types::commandBuffer parent, types::commandPool commandPool, types::queue queue ) :
        pool { commandPool }, queue { queue }
    {
    }

    commandBuffer::DATA_TYPE::~DATA_TYPE()
    {
    }
} // namespace Engine