#pragma once
#ifndef MEMORY_HXX
#    define MEMORY_HXX
#    include <common/globals.hxx>
#    include <platform.hxx>

namespace Engine
{
    struct device::_memory::DATA_TYPE
    {
        DATA_TYPE() = delete;
        DATA_TYPE( types::device device );
        ~DATA_TYPE();
        struct memoryBlock
        {
            VkDeviceSize size;
            // offset size
            std::unordered_map<VkDeviceSize, VkDeviceSize> free;
            std::unordered_map<VkDeviceSize, VkDeviceSize> allocated;
            VkDeviceMemory handle { nullptr };
        };
        bool used { false };
        // mem type <memory blocks<>>
        std::vector<std::set<memoryBlock *>> memories;
        types::device device;
    };
} // namespace Engine
#endif