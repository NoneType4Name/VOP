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
            VkDeviceSize size;
            VkDeviceSize offset;
            VkDeviceMemory memory;
            uint32_t memoryType;
            size_t memoryBlock;
        };
        memory() = delete;
        memory( device::DATA_TYPE * );

        allocationAddres allocate( VkImage image, VkMemoryPropertyFlags flags );
        allocationAddres allocate( VkBuffer buffer, VkMemoryPropertyFlags flags );
        void free( allocationAddres &addres );
        ~memory();

      private:
        struct memoryBlock
        {
            VkDeviceSize size;
            // offset size
            std::unordered_map<VkDeviceSize, VkDeviceSize> free;
            std::unordered_map<VkDeviceSize, VkDeviceSize> allocated;
            VkDeviceMemory handle;
        };
        bool used { false };
        // mem type <memory blocks<>>
        std::vector<std::vector<memoryBlock>> memories;
        device::DATA_TYPE *parent;
    };
} // namespace Engine
#endif