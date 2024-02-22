#pragma once
#ifndef MEMORY_HXX
#    define MEMORY_HXX
#    include <ObjectiveVulkan/common/globals.hxx>

namespace Engine
{
    struct device::_memory::OBJECTIVE_VULKAN_DATA_TYPE
    {
        OBJECTIVE_VULKAN_DATA_TYPE() = delete;
        OBJECTIVE_VULKAN_DATA_TYPE( types::device device );
        ~OBJECTIVE_VULKAN_DATA_TYPE();
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