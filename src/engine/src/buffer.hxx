#pragma once
#ifndef BUFFER_HXX
#    define BUFFER_HXX
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <platform.hxx>
#    include <memory.hxx>

namespace Engine
{
    class buffer
    {
      public:
        buffer() = delete;
        buffer( types::device device, VkMemoryPropertyFlags memoryPropertiesFlag, VkBufferCreateInfo BufferCreateInfo );
        void write( std::vector<void *> data, VkMemoryMapFlags flags = 0 );
        types::device device { nullptr };
        memory::allocationAddres addres;
        VkBuffer handle { nullptr };
        ~buffer();
    };

    class commandBuffer
    {
      public:
        commandBuffer( types::device device, VkCommandPool commandPool, VkCommandBufferLevel level, Engine::queue &queue );
        void begin();
        void submit();
        types::device device { nullptr };
        VkCommandPool pool { nullptr };
        VkCommandBuffer handle { nullptr };
        Engine::queue *queue { nullptr };
        VkFence fence { nullptr };
        bool used { 0 };
        ~commandBuffer();
    };
} // namespace Engine
#endif