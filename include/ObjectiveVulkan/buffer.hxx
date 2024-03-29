#pragma once
#ifndef BUFFER_HXX
#    define BUFFER_HXX
#    include <ObjectiveVulkan/common/globals.hxx>
#    include <ObjectiveVulkan/memory.hxx>

namespace Engine
{
    struct buffer::OBJECTIVE_VULKAN_DATA_TYPE
    {
        OBJECTIVE_VULKAN_DATA_TYPE( types::buffer parent, types::device device );
        ~OBJECTIVE_VULKAN_DATA_TYPE();
        types::device device { nullptr };
        device::_memory::allocationAddres addres;
        types::buffer parent { nullptr };
    };

    struct commandPool::OBJECTIVE_VULKAN_DATA_TYPE
    {
        OBJECTIVE_VULKAN_DATA_TYPE( types::commandPool parent, types::queue queue );
        ~OBJECTIVE_VULKAN_DATA_TYPE();
        types::queue queue;
        types::commandPool parent;
        std::set<types::commandBuffer> commandBuffers;
    };

    struct commandBuffer::OBJECTIVE_VULKAN_DATA_TYPE
    {
        OBJECTIVE_VULKAN_DATA_TYPE( types::commandBuffer parent, types::commandPool commandPool );
        ~OBJECTIVE_VULKAN_DATA_TYPE();
        types::commandPool pool;
        bool used { 0 };
        types::commandBuffer parent;
    };
} // namespace Engine
#endif