#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <queue.hxx>

namespace Engine
{
    class buffer
    {
      public:
        buffer( types::device device );
        buffer( VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertiesFlag, VkDeviceSize size );
        ~buffer();
        void init( VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertiesFlag, VkDeviceSize size, VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE, VkBufferCreateFlags flags = 0, void *pNext = nullptr, void *pNextAllocate = nullptr );
        VkResult map( VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags = 0 );
        void copy( void *data, VkDeviceSize size );
        void unmap();
        types::device device { nullptr };
        void *mapped { nullptr };
        VkDeviceMemory memory { nullptr };
        VkBuffer handle { nullptr };
    };

    class commandBuffer
    {
      public:
        commandBuffer( types::device device, VkCommandPool commandPool, VkCommandBufferLevel level, Engine::queue *queue );
        ~commandBuffer();
        VkCommandBuffer getHandle() const;
        void begin();
        void submit();
        void end();
        types::device device { nullptr };
        VkCommandPool pool { nullptr };
        VkCommandBuffer handle { nullptr };
        Engine::queue *queue { nullptr };
        VkFence fence { nullptr };
        bool began { 0 };
    };
} // namespace Engine