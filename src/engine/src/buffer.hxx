#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <queue.hxx>

namespace Engine
{
    namespace tools
    {
        class buffer
        {
          public:
            buffer( VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertiesFlag, VkDeviceSize size, void *data = nullptr );
            ~buffer();
            const VkBuffer getHandle();
            const VkDeviceMemory getMemoryHandle();
            VkResult map( VkDeviceSize offset, VkDeviceSize size );
            void copy( void *data, VkDeviceSize size );
            void unmap();

          private:
            void *_mapped{ nullptr };
            VkDeviceMemory _memory{ nullptr };
            VkBuffer _buffer;
        };

        class commandBuffer
        {
          public:
            commandBuffer( VkCommandPool commandPool, VkCommandBufferLevel level, queue queue );
            ~commandBuffer();
            void begin();
            void submit();
            void end();

          private:
            VkCommandPool _commandPool{ nullptr };
            VkCommandBuffer _commandBuffer{ nullptr };
            queue _queue;
            VkFence _fence{ nullptr };
            bool began{ 0 };
        };
    } // namespace tools
} // namespace Engine