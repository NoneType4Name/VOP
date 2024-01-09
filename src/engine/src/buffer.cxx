#include <buffer.hxx>
#include <device.hxx>

namespace Engine
{
    buffer::buffer( types::device device, VkMemoryPropertyFlags memoryPropertiesFlag, VkBufferCreateInfo BufferCreateInfo )
    {
        DEFINE_DATA_FIELD( device );
        BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        CHECK_RESULT( vkCreateBuffer( device->data->handle, &BufferCreateInfo, nullptr, &data->handle ) );
        data->addres = data->device->data->memory.allocate( data->handle, memoryPropertiesFlag );
    }

    buffer::~buffer()
    {
        data->device->data->memory.free( data->addres );
        vkDestroyBuffer( data->device->data->handle, data->handle, ALLOCATION_CALLBACK );
    }

    buffer::DATA_TYPE::DATA_TYPE( types::buffer parent, types::device device ) :
        parent { parent }, device { device }
    {
    }

    buffer::DATA_TYPE::~DATA_TYPE()
    {
    }

    void buffer::write( std::vector<void *> data, VkMemoryMapFlags flags )
    {
        VkBuffer TransferBuffer;
        std::vector<void *> mapped( data.size() );
        VkBufferCreateInfo bCI { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bCI.size                  = data.size();
        bCI.queueFamilyIndexCount = 1;
        bCI.pQueueFamilyIndices   = &this->data->device->data->queuesSet.transfer.familyIndex.value();
        bCI.usage                 = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bCI.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
        vkCreateBuffer( this->data->device->data->handle, &bCI, ALLOCATION_CALLBACK, &TransferBuffer );
        auto TransferBufferMemoryAddr { this->data->device->data->memory.allocate( TransferBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ) };
        vkMapMemory( this->data->device->data->handle, TransferBufferMemoryAddr.memory, TransferBufferMemoryAddr.offset, mapped.size(), flags, mapped.data() );
        memcpy( mapped.data(), data.data(), data.size() );
        vkUnmapMemory( this->data->device->data->handle, TransferBufferMemoryAddr.memory );
        VkBufferCopy copyInfo {};
        copyInfo.srcOffset = TransferBufferMemoryAddr.offset;
        copyInfo.dstOffset = this->data->addres.offset;
        copyInfo.size      = bCI.size;
        commandBuffer commands { this->data->device, this->data->device->data->transferPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, this->data->device->data->queuesSet.transfer };
        commands.begin();
        vkCmdCopyBuffer( commands.handle, TransferBuffer, this->data->handle, 1, &copyInfo );
        commands.submit();
    }

    commandBuffer::commandBuffer( types::device device, VkCommandPool commandPool, VkCommandBufferLevel level, Engine::queue &queue )
    {
        DEFINE_DATA_FIELD( device );
        data->queue = &queue;
        VkCommandBufferAllocateInfo CommandBufferAllocateInfo {};
        CommandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        CommandBufferAllocateInfo.commandPool        = commandPool; // todo
        CommandBufferAllocateInfo.level              = level;
        CommandBufferAllocateInfo.commandBufferCount = 1; // todo
        CHECK_RESULT( vkAllocateCommandBuffers( device->data->handle, &CommandBufferAllocateInfo, &handle ) );

        VkFenceCreateInfo FenceCreateInfo;
        FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        CHECK_RESULT( vkCreateFence( device->data->handle, &FenceCreateInfo, ALLOCATION_CALLBACK, &data->fence ) );
    }

    commandBuffer::~commandBuffer()
    {
        if ( data->used )
            submit();
        vkFreeCommandBuffers( data->device->data->handle, data->pool, 1, &handle );
        vkDestroyFence( data->device->data->handle, data->fence, ALLOCATION_CALLBACK );
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
        vkBeginCommandBuffer( handle, &CommandBufferBeginInfo );
    }

    void commandBuffer::submit()
    {
        if ( !data->used )
        {
            SPDLOG_CRITICAL( "Command buffer doesn't started." );
            return;
        }
        data->used = 0;
        vkEndCommandBuffer( handle );
        VkSubmitInfo SubmitInfo {};
        SubmitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers    = &handle;
        vkResetFences( data->device->data->handle, 1, &data->fence );
        vkQueueSubmit( data->queue->handle, 1, &SubmitInfo, data->fence );
        vkWaitForFences( data->device->data->handle, 1, &data->fence, true, -1ui64 );
    }

    commandBuffer::DATA_TYPE::DATA_TYPE( types::commandBuffer parent, types::device device )
    {
    }
} // namespace Engine