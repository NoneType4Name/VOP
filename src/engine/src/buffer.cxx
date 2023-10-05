#include <buffer.hxx>
#include <device.hxx>

namespace Engine
{
    namespace tools
    {
        buffer::buffer( VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertiesFlag, VkDeviceSize size, void *data )
        {
            VkBufferCreateInfo BufferCreateInfo{};
            BufferCreateInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            BufferCreateInfo.size        = size;
            BufferCreateInfo.usage       = usageFlags;
            BufferCreateInfo.flags       = BufferCreateInfo.flags ? BufferCreateInfo.flags : 0;
            BufferCreateInfo.sharingMode = BufferCreateInfo.sharingMode ? BufferCreateInfo.sharingMode : VK_SHARING_MODE_EXCLUSIVE;

            CHECK_RESULT( vkCreateBuffer( tools::getDevice(), &BufferCreateInfo, nullptr, &_buffer ) );
            VkMemoryRequirements Requirements;
            vkGetBufferMemoryRequirements( tools::getDevice(), _buffer, &Requirements );
            VkMemoryAllocateInfo MemoryAllocateInfo{};
            MemoryAllocateInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            MemoryAllocateInfo.allocationSize = Requirements.size;
            VkPhysicalDeviceMemoryProperties memProperties{};
            vkGetPhysicalDeviceMemoryProperties( tools::getPhysicalDevice(), &memProperties );

            for( uint32_t i{ 0 }; i < memProperties.memoryTypeCount; i++ )
            {
                if( ( Requirements.memoryTypeBits & ( 1 << i ) ) && ( ( memProperties.memoryTypes[ i ].propertyFlags & memoryPropertiesFlag ) == memoryPropertiesFlag ) ) MemoryAllocateInfo.memoryTypeIndex = i;
            }
            CHECK_RESULT( vkAllocateMemory( tools::getDevice(), &MemoryAllocateInfo, ALLOCATION_CALLBACK, &_memory ) );
            CHECK_RESULT( vkBindBufferMemory( tools::getDevice(), _buffer, _memory, 0 ) ); // allocate todo?
        }

        buffer::~buffer()
        {
            if( _mapped != nullptr )
                unmap();
            vkFreeMemory( tools::getDevice(), _memory, ALLOCATION_CALLBACK );
            vkDestroyBuffer( tools::getDevice(), _buffer, ALLOCATION_CALLBACK );
        }

        const VkBuffer buffer::getHandle()
        {
            return _buffer;
        }

        const VkDeviceMemory buffer::getMemoryHandle()
        {
            return _memory;
        }

        VkResult buffer::map( VkDeviceSize offset, VkDeviceSize size )
        {
            if( _mapped != nullptr )
                return VK_SUCCESS;
            return vkMapMemory( tools::getDevice(), _memory, offset, size, 0, &_mapped );
        }

        void buffer::copy( void *data, VkDeviceSize size )
        {
            if( _mapped == nullptr )
                return;
            memcpy( _mapped, data, size );
        }

        void buffer::unmap()
        {
            if( _mapped == nullptr )
                return;
            vkUnmapMemory( tools::getDevice(), _memory );
            _mapped = nullptr;
        }

        commandBuffer::commandBuffer( VkCommandPool commandPool, VkCommandBufferLevel level, queue queue )
        {
            _queue = queue;
            VkCommandBufferAllocateInfo CommandBufferAllocateInfo{};
            CommandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            CommandBufferAllocateInfo.commandPool        = commandPool;
            CommandBufferAllocateInfo.level              = level;
            CommandBufferAllocateInfo.commandBufferCount = 1;
            CHECK_RESULT( vkAllocateCommandBuffers( tools::getDevice(), &CommandBufferAllocateInfo, &_commandBuffer ) );

            VkFenceCreateInfo FenceCreateInfo;
            FenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            CHECK_RESULT( vkCreateFence( tools::getDevice(), &FenceCreateInfo, nullptr, &_fence ) );
        }

        commandBuffer::~commandBuffer()
        {
            vkFreeCommandBuffers( tools::getDevice(), _commandPool, 1, &_commandBuffer );
            vkDestroyFence( tools::getDevice(), _fence, ALLOCATION_CALLBACK );
        }

        VkCommandBuffer commandBuffer::getHandle() const
        {
            return _commandBuffer;
        }

        void commandBuffer::begin()
        {
            if( began ) return;
            VkCommandBufferBeginInfo CommandBufferBeginInfo{};
            CommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            CommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            vkBeginCommandBuffer( _commandBuffer, &CommandBufferBeginInfo );
        }

        void commandBuffer::submit()
        {
            end();
            VkSubmitInfo SubmitInfo{};
            SubmitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            SubmitInfo.commandBufferCount = 1;
            SubmitInfo.pCommandBuffers    = &_commandBuffer;
            vkResetFences( tools::getDevice(), 1, &_fence );
            vkQueueSubmit( _queue.GetHandle(), 1, &SubmitInfo, _fence );
            vkWaitForFences( tools::getDevice(), 1, &_fence, true, -1ui64 );
        }

        void commandBuffer::end()
        {
            if( began )
                vkEndCommandBuffer( _commandBuffer );
        }
    } // namespace tools
} // namespace Engine