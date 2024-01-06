#pragma once
#ifndef MEMORY_HXX
#    define MEMORY_HXX
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <platform.hxx>

namespace Engine
{
    struct memory
    {
        struct allocationAddres
        {
            uint32_t memoryType;
            size_t memoryBlock;
            VkDeviceSize offset;
        };
        memory() = delete;
        memory( device::DATA_TYPE * );

        allocationAddres allocate( types::image image );
        // uint32_t allocate( types::buffer );
        void free( types::image image );
        // void free( types::buffer buffer );
        ~memory();

      private:
        struct memoryBlock
        {
            VkDeviceSize size;
            VkDeviceSize freeSize;
            // offset size
            std::unordered_map<VkDeviceSize, VkDeviceSize> free;
            std::unordered_map<VkDeviceSize, VkDeviceSize> allocated;
            VkDeviceMemory handle;
        };
        // mem type <memory blocks<>>
        std::vector<std::vector<memoryBlock>> memories;
        device::DATA_TYPE *parent;
    };
} // namespace Engine
#endif