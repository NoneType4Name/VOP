#include <buffer.hxx>
#include <device.hxx>

namespace Engine
{
    buffer::buffer( types::device device, VkMemoryPropertyFlags memoryPropertiesFlag, VkBufferCreateInfo BufferCreateInfo ) :
        device { device }
    {
        BufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        CHECK_RESULT( vkCreateBuffer( device->data->handle, &BufferCreateInfo, nullptr, &handle ) );
        addres = this->device->data->memory.allocate( handle, memoryPropertiesFlag );
    }

    buffer::~buffer()
    {
        this->device->data->memory.free( addres );
        vkDestroyBuffer( device->data->handle, handle, ALLOCATION_CALLBACK );
    }

    void buffer::write( std::vector<void *> data, VkMemoryMapFlags flags )
    {
        VkBuffer TransferBuffer;
        std::vector<void *> mapped( data.size() );
        VkBufferCreateInfo bCI { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
        bCI.size                  = data.size();
        bCI.queueFamilyIndexCount = 1;
        bCI.pQueueFamilyIndices   = &device->data->queuesSet.transfer.familyIndex.value();
        bCI.usage                 = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bCI.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
        vkCreateBuffer( device->data->handle, &bCI, ALLOCATION_CALLBACK, &TransferBuffer );
        auto TransferBufferMemoryAddr { device->data->memory.allocate( TransferBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ) };
        vkMapMemory( device->data->handle, TransferBufferMemoryAddr.memory, TransferBufferMemoryAddr.offset, mapped.size(), flags, mapped.data() );
        memcpy( mapped.data(), data.data(), data.size() );
        vkUnmapMemory( device->data->handle, TransferBufferMemoryAddr.memory );
        VkBufferCopy copyInfo {};
        copyInfo.srcOffset = TransferBufferMemoryAddr.offset;
        copyInfo.dstOffset = addres.offset;
        copyInfo.size      = bCI.size;
        commandBuffer commands { device, device->data->transferPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, device->data->queuesSet.transfer };
        commands.begin();
        vkCmdCopyBuffer( commands.handle, TransferBuffer, handle, 1, &copyInfo );
        commands.submit();
    }

    commandBuffer::commandBuffer( types::device device, VkCommandPool commandPool, VkCommandBufferLevel level, Engine::queue &queue )
    {
        this->device = device;
        this->queue  = &queue;
        VkCommandBufferAllocateInfo CommandBufferAllocateInfo {};
        CommandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        CommandBufferAllocateInfo.commandPool        = commandPool; // todo
        CommandBufferAllocateInfo.level              = level;
        CommandBufferAllocateInfo.commandBufferCount = 1; // todo
        CHECK_RESULT( vkAllocateCommandBuffers( device->data->handle, &CommandBufferAllocateInfo, &handle ) );

        VkFenceCreateInfo FenceCreateInfo;
        FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        CHECK_RESULT( vkCreateFence( device->data->handle, &FenceCreateInfo, ALLOCATION_CALLBACK, &fence ) );
    }

    commandBuffer::~commandBuffer()
    {
        if ( used )
            submit();
        vkFreeCommandBuffers( device->data->handle, pool, 1, &handle );
        vkDestroyFence( device->data->handle, fence, ALLOCATION_CALLBACK );
    }

    void commandBuffer::begin()
    {
        if ( used )
        {
            SPDLOG_CRITICAL( "Allready begined command buffer." );
            return;
        };
        used = 1;
        VkCommandBufferBeginInfo CommandBufferBeginInfo {};
        CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer( handle, &CommandBufferBeginInfo );
    }

    void commandBuffer::submit()
    {
        if ( !used )
        {
            SPDLOG_CRITICAL( "Command buffer doesn't started." );
            return;
        }
        used = 0;
        vkEndCommandBuffer( handle );
        VkSubmitInfo SubmitInfo {};
        SubmitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        SubmitInfo.commandBufferCount = 1;
        SubmitInfo.pCommandBuffers    = &handle;
        vkResetFences( device->data->handle, 1, &fence );
        vkQueueSubmit( queue->handle, 1, &SubmitInfo, fence );
        vkWaitForFences( device->data->handle, 1, &fence, true, -1ui64 );
    }
} // namespace Engine