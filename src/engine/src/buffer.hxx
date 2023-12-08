#pragma once
#ifndef BUFFER_HXX
#    define BUFFER_HXX
#    include <platform.hxx>
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <EHI.hxx>

namespace Engine
{
    class buffer
    {
      public:
        buffer( types::device device );
        buffer( types::device device, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertiesFlag, VkDeviceSize size, VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE, VkBufferCreateFlags flags = 0, void *pNext = nullptr );
        void init( VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertiesFlag, VkDeviceSize size, VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE, VkBufferCreateFlags flags = 0, void *pNext = nullptr );
        void copy( void *data, VkDeviceSize size, VkMemoryMapFlags flags = 0 );
        types::device device { nullptr };
        void *mapped { nullptr };
        VkBuffer handle { nullptr };
        ~buffer();
    };

    class commandBuffer
    {
      public:
        commandBuffer( types::device device, VkCommandPool commandPool, VkCommandBufferLevel level, Engine::queue *queue );
        void begin();
        void submit();
        void end();
        types::device device { nullptr };
        VkCommandPool pool { nullptr };
        VkCommandBuffer handle { nullptr };
        Engine::queue *queue { nullptr };
        VkFence fence { nullptr };
        bool began { 0 };
        ~commandBuffer();
    };
} // namespace Engine
#endif