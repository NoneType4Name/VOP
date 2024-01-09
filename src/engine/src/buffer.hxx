#pragma once
#ifndef BUFFER_HXX
#    define BUFFER_HXX
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <platform.hxx>
#    include <memory.hxx>

namespace Engine
{
    class buffer::DATA_TYPE
    {
      public:
        DATA_TYPE( types::buffer parent, types::device device );
        ~DATA_TYPE();
        types::device device { nullptr };
        memory::allocationAddres addres;
        types::buffer parent { nullptr };
    };

    class commandBuffer::DATA_TYPE
    {
      public:
        DATA_TYPE( types::commandBuffer parent, types::device device, VkCommandPool commandPool, Engine::queue &queue );
        ~DATA_TYPE();
        types::device device { nullptr };
        VkCommandPool pool { nullptr };
        Engine::queue *queue { nullptr };
        VkFence fence { nullptr };
        bool used { 0 };
        types::commandBuffer parent { nullptr };
    };
} // namespace Engine
#endif