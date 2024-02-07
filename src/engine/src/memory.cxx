#include <memory.hxx>
#include <device.hxx>
#include <common/logging.hxx>

namespace Engine
{
    device::_memory::_memory( types::device device )
    {
        const_cast<std::unique_ptr<DATA_TYPE> &>( data ) = std::make_unique<DATA_TYPE>( device );
    }

    device::_memory::~_memory()
    {
        for ( auto &memType : data->memories )
            for ( auto &block : memType )
            {
                if ( block->handle )
                    vkFreeMemory( data->device->handle, block->handle, ENGINE_ALLOCATION_CALLBACK );
                delete block;
            }
    }

    device::_memory::DATA_TYPE::DATA_TYPE( types::device device ) :
        device { device }
    {
    }

    device::_memory::DATA_TYPE::~DATA_TYPE()
    {
    }

    device::_memory::allocationAddres device::_memory::allocate( VkImage image, VkMemoryPropertyFlags flags )
    {
        VkMemoryRequirements mReq;
        vkGetImageMemoryRequirements( data->device->handle, image, &mReq );
        allocationAddres ret {};
        ret.size       = mReq.size;
        ret.memoryType = data->device->requeredMemoryTypeIndex( mReq.memoryTypeBits, flags );
        while ( data->used ) continue;
        data->used = 1;
        if ( data->memories.empty() )
            data->memories.resize( data->device->data->description->data->memProperties.memoryTypeCount );
        auto &memType { data->memories[ ret.memoryType ] };
        for ( auto memBlock : memType )
        {
            std::pair<VkDeviceSize, VkDeviceSize> min { 0, ~0Ui64 };
            ret.block = memBlock;
            for ( auto &freeSize : memBlock->free )
                if ( freeSize.second > mReq.size + ( freeSize.first % mReq.alignment ) && freeSize.second < min.second )
                {
                    min = freeSize;
                }
            if ( !~min.second )
                break;
            if ( min.first % mReq.alignment )
            {
                memBlock->free[ min.first ]                                    = mReq.alignment - min.first % mReq.alignment;
                memBlock->allocated[ min.first + memBlock->free[ min.first ] ] = mReq.size;
            }
            else
            {
                ret.offset                        = min.first;
                memBlock->allocated[ ret.offset ] = mReq.size;
                if ( min.second > mReq.size )
                    memBlock->free[ ret.offset + mReq.size ] = min.second - mReq.size;
                memBlock->free.erase( ret.offset );
            }
            ret.memory = memBlock->handle;
            vkBindImageMemory( data->device->handle, image, memBlock->handle, ret.offset );
            data->used = 0;
            return ret;
        }
        VkMemoryAllocateInfo info { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        // new block
        info.memoryTypeIndex = ret.memoryType;
        info.allocationSize  = ( data->device->data->description->data->memProperties.memoryHeaps[ data->device->data->description->data->memProperties.memoryTypes[ ret.memoryType ].heapIndex ].size ) / ( data->device->data->description->data->properties.limits.maxMemoryAllocationCount / data->device->data->description->data->memProperties.memoryTypeCount );
        uint32_t MemMultiply { 0 };
        while ( mReq.size > info.allocationSize * ++MemMultiply )
            continue;
        info.allocationSize *= MemMultiply;
        auto memBlock { const_cast<device::_memory::DATA_TYPE::memoryBlock *>( *memType.insert( new device::_memory::DATA_TYPE::memoryBlock { .size = info.allocationSize } ).first ) };
        memBlock->allocated[ 0 ]    = mReq.size + ( mReq.size % mReq.alignment );
        memBlock->free[ mReq.size ] = memBlock->size - mReq.size - ( mReq.size % mReq.alignment );
        ret.block                   = memBlock;
        ret.offset                  = 0;
        data->used                  = 0;
        vkAllocateMemory( data->device->handle, &info, ENGINE_ALLOCATION_CALLBACK, &memBlock->handle );
        ret.memory = memBlock->handle;
        vkBindImageMemory( data->device->handle, image, ret.memory, ret.offset );
        return ret;
    }

    device::_memory::allocationAddres device::_memory::allocate( VkBuffer buffer, VkMemoryPropertyFlags flags )
    {
        VkMemoryRequirements mReq;
        vkGetBufferMemoryRequirements( data->device->handle, buffer, &mReq );
        allocationAddres ret {};
        ret.size       = mReq.size;
        ret.memoryType = data->device->requeredMemoryTypeIndex( mReq.memoryTypeBits, flags );
        while ( data->used ) continue;
        data->used = 1;
        if ( data->memories.empty() )
            data->memories.resize( data->device->data->description->data->memProperties.memoryTypeCount );
        auto &memType { data->memories[ ret.memoryType ] };
        for ( auto memBlock : memType )
        {
            std::pair<VkDeviceSize, VkDeviceSize> min { 0, ~0Ui64 };
            ret.block = memBlock;
            for ( auto &freeSize : memBlock->free )
                if ( freeSize.second > mReq.size + ( freeSize.first % mReq.alignment ) && freeSize.second < min.second )
                {
                    min = freeSize;
                }
            if ( !~min.second )
                break;
            if ( min.first % mReq.alignment )
            {
                memBlock->free[ min.first ]                                    = mReq.alignment - min.first % mReq.alignment;
                memBlock->allocated[ min.first + memBlock->free[ min.first ] ] = mReq.size;
            }
            else
            {
                ret.offset                        = min.first;
                memBlock->allocated[ ret.offset ] = mReq.size;
                if ( min.second > mReq.size )
                    memBlock->free[ ret.offset + mReq.size ] = min.second - mReq.size;
                memBlock->free.erase( ret.offset );
            }
            ret.memory = memBlock->handle;
            vkBindBufferMemory( data->device->handle, buffer, memBlock->handle, ret.offset );
            data->used = 0;
            return ret;
        }
        VkMemoryAllocateInfo info { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        // new block
        info.memoryTypeIndex = ret.memoryType;
        info.allocationSize  = ( data->device->data->description->data->memProperties.memoryHeaps[ data->device->data->description->data->memProperties.memoryTypes[ ret.memoryType ].heapIndex ].size ) / ( data->device->data->description->data->properties.limits.maxMemoryAllocationCount / data->device->data->description->data->memProperties.memoryTypeCount );
        uint32_t MemMultiply { 0 };
        while ( mReq.size > info.allocationSize * ++MemMultiply )
            continue;
        info.allocationSize *= MemMultiply;
        auto memBlock { const_cast<device::_memory::DATA_TYPE::memoryBlock *>( *memType.insert( new device::_memory::DATA_TYPE::memoryBlock { .size = info.allocationSize } ).first ) };
        memBlock->allocated[ 0 ]    = mReq.size + ( mReq.size % mReq.alignment );
        memBlock->free[ mReq.size ] = memBlock->size - mReq.size - ( mReq.size % mReq.alignment );
        ret.block                   = memBlock;
        ret.offset                  = 0;
        data->used                  = 0;
        vkAllocateMemory( data->device->handle, &info, ENGINE_ALLOCATION_CALLBACK, &memBlock->handle );
        ret.memory = memBlock->handle;
        vkBindBufferMemory( data->device->handle, buffer, ret.memory, ret.offset );
        return ret;
    }

    void device::_memory::free( _memory::allocationAddres &addr )
    {
        while ( data->used ) continue;
        data->used = 1;
        // error: free blocks and removed indecies.
        auto block { static_cast<device::_memory::DATA_TYPE::memoryBlock *>( addr.block ) };
        block->allocated.erase( addr.offset );
        if ( block->allocated.empty() )
        {
            vkFreeMemory( data->device->handle, block->handle, ENGINE_ALLOCATION_CALLBACK );
            delete block;
            data->memories[ addr.memoryType ].erase( block );
            goto ret;
        }
        if ( block->free.contains( addr.offset + addr.size ) )
            addr.size += block->free[ addr.offset + addr.size ];
        if ( addr.offset > addr.size && block->free.contains( addr.offset - addr.size ) )
            block->free[ addr.offset - addr.size ] += addr.size;
        else
            block->free[ addr.offset ] = addr.size;
    ret:
        data->used  = 0;
        addr.memory = VK_NULL_HANDLE;
    }
    // uint32_t device::DATA_TYPE::setImageMemory( VkImage &image, VkMemoryPropertyFlags properties )
    // {
    //     if ( !images.size() )
    //         images.resize( description->memProperties.memoryTypeCount );
    //     VkMemoryRequirements mReq {};
    //     VkMemoryAllocateInfo allocateInfo {};
    //     allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    //     vkGetImageMemoryRequirements( handle, image, &mReq );
    //     allocateInfo.memoryTypeIndex                          = tools::requeredMemoryTypeIndex( description->memProperties, mReq.memoryTypeBits, properties );
    //     images[ allocateInfo.memoryTypeIndex ].first[ image ] = mReq.size;
    //     if ( images[ allocateInfo.memoryTypeIndex ].second.second < images[ allocateInfo.memoryTypeIndex ].second.first + mReq.size )
    //     {
    //         images[ allocateInfo.memoryTypeIndex ].second.second = allocateInfo.allocationSize = images[ allocateInfo.memoryTypeIndex ].second.first + mReq.size;
    //         vkFreeMemory( handle, imagesMemory, ENGINE_ALLOCATION_CALLBACK );
    //         vkAllocateMemory( handle, &allocateInfo, ENGINE_ALLOCATION_CALLBACK, &imagesMemory );
    //         images[ allocateInfo.memoryTypeIndex ].second.first = 0;
    //         for ( const auto img : images[ allocateInfo.memoryTypeIndex ].first )
    //         {
    //             vkBindImageMemory( handle, img.first, imagesMemory, images[ allocateInfo.memoryTypeIndex ].second.first );
    //             images[ allocateInfo.memoryTypeIndex ].second.first += img.second;
    //         }
    //     }
    //     else
    //     {
    //         vkBindImageMemory( handle, image, imagesMemory, images[ allocateInfo.memoryTypeIndex ].second.first );
    //         images[ allocateInfo.memoryTypeIndex ].second.first += mReq.size;
    //     }
    //     return allocateInfo.memoryTypeIndex;
    // }

    // void device::DATA_TYPE::addImagesMemorySize( uint32_t index, uint32_t size )
    // {
    //     VkMemoryAllocateInfo allocateInfo {};
    //     allocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    //     allocateInfo.memoryTypeIndex = index;
    //     // if
    //     allocateInfo.allocationSize                         = images[ allocateInfo.memoryTypeIndex ].second.second += size;
    //     images[ allocateInfo.memoryTypeIndex ].second.first = 0;
    //     vkFreeMemory( handle, imagesMemory, ENGINE_ALLOCATION_CALLBACK );
    //     vkAllocateMemory( handle, &allocateInfo, ENGINE_ALLOCATION_CALLBACK, &imagesMemory );
    //     for ( auto &buf : images[ allocateInfo.memoryTypeIndex ].first )
    //     {
    //         vkBindImageMemory( handle, buf.first, imagesMemory, images[ allocateInfo.memoryTypeIndex ].second.first );
    //         images[ allocateInfo.memoryTypeIndex ].second.first += buf.second;
    //     }
    // }

    // uint32_t device::DATA_TYPE::allocateBufferMemory( VkBuffer &buffer, VkMemoryPropertyFlags properties )
    // {
    //     if ( !buffers.size() )
    //         buffers.resize( description->memProperties.memoryTypeCount );
    //     VkMemoryRequirements mReq {};
    //     VkMemoryAllocateInfo allocateInfo {};
    //     allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    //     vkGetBufferMemoryRequirements( handle, buffer, &mReq );
    //     allocateInfo.memoryTypeIndex                            = tools::requeredMemoryTypeIndex( description->memProperties, mReq.memoryTypeBits, properties );
    //     buffers[ allocateInfo.memoryTypeIndex ].first[ buffer ] = mReq.size;
    //     if ( buffers[ allocateInfo.memoryTypeIndex ].second.second < buffers[ allocateInfo.memoryTypeIndex ].second.first + mReq.size )
    //     {
    //         buffers[ allocateInfo.memoryTypeIndex ].second.second = allocateInfo.allocationSize = buffers[ allocateInfo.memoryTypeIndex ].second.first + mReq.size;
    //         vkFreeMemory( handle, buffersMemory, ENGINE_ALLOCATION_CALLBACK );
    //         vkAllocateMemory( handle, &allocateInfo, ENGINE_ALLOCATION_CALLBACK, &buffersMemory );
    //         buffers[ allocateInfo.memoryTypeIndex ].second.first = 0;
    //         for ( const auto img : buffers[ allocateInfo.memoryTypeIndex ].first )
    //         {
    //             vkBindBufferMemory( handle, img.first, buffersMemory, buffers[ allocateInfo.memoryTypeIndex ].second.first );
    //             buffers[ allocateInfo.memoryTypeIndex ].second.first += img.second;
    //         }
    //     }
    //     else
    //     {
    //         vkBindBufferMemory( handle, buffer, buffersMemory, buffers[ allocateInfo.memoryTypeIndex ].second.first );
    //         buffers[ allocateInfo.memoryTypeIndex ].second.first += mReq.size;
    //     }
    //     return allocateInfo.memoryTypeIndex;
    // }

    // void device::DATA_TYPE::addBuffersMemorySize( uint32_t index, uint32_t size )
    // {
    //     VkMemoryAllocateInfo allocateInfo {};
    //     allocateInfo.sType                                   = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    //     allocateInfo.memoryTypeIndex                         = index;
    //     allocateInfo.allocationSize                          = buffers[ allocateInfo.memoryTypeIndex ].second.second += size;
    //     buffers[ allocateInfo.memoryTypeIndex ].second.first = 0;
    //     vkFreeMemory( handle, buffersMemory, ENGINE_ALLOCATION_CALLBACK );
    //     vkAllocateMemory( handle, &allocateInfo, ENGINE_ALLOCATION_CALLBACK, &buffersMemory );
    //     for ( auto &buf : buffers[ allocateInfo.memoryTypeIndex ].first )
    //     {
    //         vkBindBufferMemory( handle, buf.first, buffersMemory, buffers[ allocateInfo.memoryTypeIndex ].second.first );
    //         buffers[ allocateInfo.memoryTypeIndex ].second.first += buf.second;
    //     }
    // }

    // void device::DATA_TYPE::allocateBufferMemory( VkBuffer buffer, VkMemoryPropertyFlags properties )
    // {
    //     if ( !buffers.size() )
    //         buffers.resize( description->memProperties.memoryTypeCount );
    //     VkMemoryRequirements mReq {};
    //     VkMemoryAllocateInfo allocateInfo {};
    //     allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    //     vkGetBufferMemoryRequirements( handle, buffer, &mReq );
    //     allocateInfo.memoryTypeIndex                            = tools::requeredMemoryTypeIndex( description->memProperties, mReq.memoryTypeBits, properties );
    //     buffers[ allocateInfo.memoryTypeIndex ].first[ buffer ] = mReq.size;
    //     while ( !buffersMemory )
    //         continue;
    //     if ( buffers[ allocateInfo.memoryTypeIndex ].second.second < buffers[ allocateInfo.memoryTypeIndex ].second.first + mReq.size )
    //     {
    //         buffers[ allocateInfo.memoryTypeIndex ].second.second = allocateInfo.allocationSize = buffers[ allocateInfo.memoryTypeIndex ].second.first + mReq.size;
    //         vkFreeMemory( handle, buffersMemory, ENGINE_ALLOCATION_CALLBACK );
    //         vkAllocateMemory( handle, &allocateInfo, ENGINE_ALLOCATION_CALLBACK, &buffersMemory );
    //         buffers[ allocateInfo.memoryTypeIndex ].second.first = 0;
    //         for ( const auto img : buffers[ allocateInfo.memoryTypeIndex ].first )
    //         {
    //             vkBindBufferMemory( handle, img.first, buffersMemory, buffers[ allocateInfo.memoryTypeIndex ].second.first );
    //             buffers[ allocateInfo.memoryTypeIndex ].second.first += img.second;
    //         }
    //     }
    //     else
    //     {
    //         vkBindBufferMemory( handle, buffer, buffersMemory, buffers[ allocateInfo.memoryTypeIndex ].second.first );
    //         buffers[ allocateInfo.memoryTypeIndex ].second.first += mReq.size;
    //     }
    // }

    // void device::DATA_TYPE::writeBufferMemory( VkBuffer buf, VkMemoryMapFlags flags, void **data, VkDeviceSize size )
    // {
    //     buffer b {};
    // }

    // void device::DATA_TYPE::freeBufferMemory( VkBuffer buffer )
    // {
    // }
} // namespace Engine