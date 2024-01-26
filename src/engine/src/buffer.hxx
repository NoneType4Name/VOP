#pragma once
#ifndef BUFFER_HXX
#    define BUFFER_HXX
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <platform.hxx>
#    include <memory.hxx>

namespace Engine
{
    struct buffer::DATA_TYPE
    {
        DATA_TYPE( types::buffer parent, types::device device );
        ~DATA_TYPE();
        types::device device { nullptr };
        device::memory::allocationAddres addres;
        types::buffer parent { nullptr };
    };

    struct commandPool::DATA_TYPE
    {
        DATA_TYPE( types::commandPool parent, types::queue queue );
        ~DATA_TYPE();
        types::queue queue;
        types::commandPool parent;
        std::set<types::commandBuffer> commandBuffers;
    };

    struct commandBuffer::DATA_TYPE
    {
        DATA_TYPE( types::commandBuffer parent, types::commandPool commandPool, types::queue queue );
        ~DATA_TYPE();
        types::commandPool pool;
        types::queue queue;
        bool used { 0 };
        types::commandBuffer parent;
    };
} // namespace Engine
#endif