#include <memory.hxx>
#include <device.hxx>

namespace Engine
{
    memory::memory( device::DATA_TYPE *parent ) :
        parent { parent }
    {
    }

    memory::~memory()
    {
        for ( auto &memType : memories )
            for ( auto &block : memType )
                if ( block.handle )
                    vkFreeMemory( parent->handle, block.handle, ALLOCATION_CALLBACK );
    }

    memory::allocationAddres memory::allocate( VkImage image, VkMemoryPropertyFlags flags )
    {
        VkMemoryRequirements mReq;
        vkGetImageMemoryRequirements( parent->handle, image, &mReq );
        allocationAddres ret { parent->requeredMemoryTypeIndex( mReq.memoryTypeBits, flags ) };
        while ( used ) continue;
        used = 1;
        if ( memories.empty() )
            memories.resize( parent->description->data->memProperties.memoryTypeCount );
        auto &memType { memories[ ret.memoryType ] };
        for ( size_t block { 0 }; block < memType.size(); ++block )
        {
            std::pair<VkDeviceSize, VkDeviceSize> min { 0, 0 };
            ret.memoryBlock = block;
            for ( auto &freeSize : memType[ block ].free )
                if ( freeSize.second > mReq.size + mReq.alignment && freeSize.second < min.second )
                {
                    min = freeSize;
                }
            if ( min.first % mReq.alignment )
            {
                memType[ block ].free[ min.first ]                                           = mReq.alignment - min.first % mReq.alignment;
                memType[ block ].allocated[ min.first + memType[ block ].free[ min.first ] ] = mReq.size;
            }
            else
            {
                ret.offset                               = min.first;
                memType[ block ].allocated[ ret.offset ] = mReq.size;
                if ( min.second > mReq.size )
                    memType[ block ].free[ ret.offset + mReq.size ] = min.second - mReq.size;
                memType[ block ].free.erase( ret.offset );
            }
            ret.memory = memType[ block ].handle;
            vkBindImageMemory( parent->handle, image, memType[ block ].handle, ret.offset );
            return ret;
        }
        VkMemoryAllocateInfo info { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        // new block
        memType.emplace_back();
        info.memoryTypeIndex = ret.memoryType;
        info.allocationSize  = ( parent->description->data->memProperties.memoryHeaps[ parent->description->data->memProperties.memoryTypes[ ret.memoryType ].heapIndex ].size ) / parent->description->data->properties.limits.maxMemoryAllocationCount / parent->description->data->memProperties.memoryTypeCount;
        uint32_t MemMultiply { 0 };
        while ( mReq.size + mReq.alignment > info.allocationSize * ++MemMultiply )
            continue;
        info.allocationSize *= MemMultiply;
        memType.back().size                               = info.allocationSize;
        memType.back().allocated[ 0 ]                     = info.allocationSize;
        memType.back().free[ mReq.size + mReq.alignment ] = memType.back().size - mReq.size - mReq.alignment;
        ret.memoryBlock                                   = memType.size();
        ret.offset                                        = 0;
        used                                              = 0;
        vkAllocateMemory( parent->handle, &info, ALLOCATION_CALLBACK, &memType.back().handle );
        ret.memory = memType.back().handle;
        vkBindImageMemory( parent->handle, image, ret.memory, ret.offset );
        return ret;
    }

    memory::allocationAddres memory::allocate( VkBuffer buffer, VkMemoryPropertyFlags flags )
    {
        VkMemoryRequirements mReq;
        vkGetBufferMemoryRequirements( parent->handle, buffer, &mReq );
        allocationAddres ret { parent->requeredMemoryTypeIndex( mReq.memoryTypeBits, flags ) };
        while ( used ) continue;
        used = 1;
        auto &memType { memories[ ret.memoryType ] };
        for ( size_t block { 0 }; block < memType.size(); ++block )
        {
            std::pair<VkDeviceSize, VkDeviceSize> min { 0, 0 };
            ret.memoryBlock = block;
            for ( auto &freeSize : memType[ block ].free )
                if ( freeSize.second > mReq.size + mReq.alignment && freeSize.second < min.second )
                {
                    min = freeSize;
                }
            if ( min.first % mReq.alignment )
            {
                memType[ block ].free[ min.first ]                                           = mReq.alignment - min.first % mReq.alignment;
                memType[ block ].allocated[ min.first + memType[ block ].free[ min.first ] ] = mReq.size;
            }
            else
            {
                ret.offset                               = min.first;
                memType[ block ].allocated[ ret.offset ] = mReq.size;
                if ( min.second > mReq.size )
                    memType[ block ].free[ ret.offset + mReq.size ] = min.second - mReq.size;
                memType[ block ].free.erase( ret.offset );
            }
            ret.memory = memType[ block ].handle;
            vkBindBufferMemory( parent->handle, buffer, memType[ block ].handle, ret.offset );
            return ret;
        }
        VkMemoryAllocateInfo info { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
        // new block
        memType.emplace_back();
        info.memoryTypeIndex = ret.memoryType;
        info.allocationSize  = ( parent->description->data->memProperties.memoryHeaps[ parent->description->data->memProperties.memoryTypes[ ret.memoryType ].heapIndex ].size ) / parent->description->data->properties.limits.maxMemoryAllocationCount / parent->description->data->memProperties.memoryTypeCount;
        uint32_t MemMultiply { 0 };
        while ( mReq.size + mReq.alignment > info.allocationSize * ++MemMultiply )
            continue;
        info.allocationSize *= MemMultiply;
        memType.back().size                               = info.allocationSize;
        memType.back().allocated[ 0 ]                     = info.allocationSize;
        memType.back().free[ mReq.size + mReq.alignment ] = memType.back().size - mReq.size - mReq.alignment;
        ret.memoryBlock                                   = memType.size();
        ret.offset                                        = 0;
        used                                              = 0;
        vkAllocateMemory( parent->handle, &info, ALLOCATION_CALLBACK, &memType.back().handle );
        ret.memory = memType.back().handle;
        vkBindBufferMemory( parent->handle, buffer, ret.memory, ret.offset );
        return ret;
    }

    void memory::free( memory::allocationAddres &addr )
    {
        while ( used ) continue;
        used = 1;
        auto &mType { memories[ addr.memoryType ] };
        auto &mBlock { mType[ addr.memoryBlock ] };
        mBlock.allocated.erase( addr.offset );
        if ( mBlock.allocated.empty() )
        {
            vkFreeMemory( parent->handle, mBlock.handle, ALLOCATION_CALLBACK );
            mType.erase( mType.begin() + addr.memoryBlock );
            goto ret;
        }
        if ( mBlock.free.contains( addr.offset + addr.size ) )
            addr.size += mBlock.free[ addr.offset + addr.size ];
        if ( addr.offset > addr.size && mBlock.free.contains( addr.offset - addr.size ) )
            mBlock.free[ addr.offset - addr.size ] += addr.size;
        else
            mBlock.free[ addr.offset ] = addr.size;
    ret:
        used        = 0;
        addr.memory = VK_NULL_HANDLE;
    }
    // uint32_t device::DATA_TYPE::setImageMemory( VkImage &image, VkMemoryPropertyFlags properties )
    // {
    //     if ( !images.size() )
    //         images.resize( description->data->memProperties.memoryTypeCount );
    //     VkMemoryRequirements mReq {};
    //     VkMemoryAllocateInfo allocateInfo {};
    //     allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    //     vkGetImageMemoryRequirements( handle, image, &mReq );
    //     allocateInfo.memoryTypeIndex                          = tools::requeredMemoryTypeIndex( description->data->memProperties, mReq.memoryTypeBits, properties );
    //     images[ allocateInfo.memoryTypeIndex ].first[ image ] = mReq.size;
    //     if ( images[ allocateInfo.memoryTypeIndex ].second.second < images[ allocateInfo.memoryTypeIndex ].second.first + mReq.size )
    //     {
    //         images[ allocateInfo.memoryTypeIndex ].second.second = allocateInfo.allocationSize = images[ allocateInfo.memoryTypeIndex ].second.first + mReq.size;
    //         vkFreeMemory( handle, imagesMemory, ALLOCATION_CALLBACK );
    //         vkAllocateMemory( handle, &allocateInfo, ALLOCATION_CALLBACK, &imagesMemory );
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
    //     vkFreeMemory( handle, imagesMemory, ALLOCATION_CALLBACK );
    //     vkAllocateMemory( handle, &allocateInfo, ALLOCATION_CALLBACK, &imagesMemory );
    //     for ( auto &buf : images[ allocateInfo.memoryTypeIndex ].first )
    //     {
    //         vkBindImageMemory( handle, buf.first, imagesMemory, images[ allocateInfo.memoryTypeIndex ].second.first );
    //         images[ allocateInfo.memoryTypeIndex ].second.first += buf.second;
    //     }
    // }

    // uint32_t device::DATA_TYPE::allocateBufferMemory( VkBuffer &buffer, VkMemoryPropertyFlags properties )
    // {
    //     if ( !buffers.size() )
    //         buffers.resize( description->data->memProperties.memoryTypeCount );
    //     VkMemoryRequirements mReq {};
    //     VkMemoryAllocateInfo allocateInfo {};
    //     allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    //     vkGetBufferMemoryRequirements( handle, buffer, &mReq );
    //     allocateInfo.memoryTypeIndex                            = tools::requeredMemoryTypeIndex( description->data->memProperties, mReq.memoryTypeBits, properties );
    //     buffers[ allocateInfo.memoryTypeIndex ].first[ buffer ] = mReq.size;
    //     if ( buffers[ allocateInfo.memoryTypeIndex ].second.second < buffers[ allocateInfo.memoryTypeIndex ].second.first + mReq.size )
    //     {
    //         buffers[ allocateInfo.memoryTypeIndex ].second.second = allocateInfo.allocationSize = buffers[ allocateInfo.memoryTypeIndex ].second.first + mReq.size;
    //         vkFreeMemory( handle, buffersMemory, ALLOCATION_CALLBACK );
    //         vkAllocateMemory( handle, &allocateInfo, ALLOCATION_CALLBACK, &buffersMemory );
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
    //     vkFreeMemory( handle, buffersMemory, ALLOCATION_CALLBACK );
    //     vkAllocateMemory( handle, &allocateInfo, ALLOCATION_CALLBACK, &buffersMemory );
    //     for ( auto &buf : buffers[ allocateInfo.memoryTypeIndex ].first )
    //     {
    //         vkBindBufferMemory( handle, buf.first, buffersMemory, buffers[ allocateInfo.memoryTypeIndex ].second.first );
    //         buffers[ allocateInfo.memoryTypeIndex ].second.first += buf.second;
    //     }
    // }

    // void device::DATA_TYPE::allocateBufferMemory( VkBuffer buffer, VkMemoryPropertyFlags properties )
    // {
    //     if ( !buffers.size() )
    //         buffers.resize( description->data->memProperties.memoryTypeCount );
    //     VkMemoryRequirements mReq {};
    //     VkMemoryAllocateInfo allocateInfo {};
    //     allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    //     vkGetBufferMemoryRequirements( handle, buffer, &mReq );
    //     allocateInfo.memoryTypeIndex                            = tools::requeredMemoryTypeIndex( description->data->memProperties, mReq.memoryTypeBits, properties );
    //     buffers[ allocateInfo.memoryTypeIndex ].first[ buffer ] = mReq.size;
    //     while ( !buffersMemory )
    //         continue;
    //     if ( buffers[ allocateInfo.memoryTypeIndex ].second.second < buffers[ allocateInfo.memoryTypeIndex ].second.first + mReq.size )
    //     {
    //         buffers[ allocateInfo.memoryTypeIndex ].second.second = allocateInfo.allocationSize = buffers[ allocateInfo.memoryTypeIndex ].second.first + mReq.size;
    //         vkFreeMemory( handle, buffersMemory, ALLOCATION_CALLBACK );
    //         vkAllocateMemory( handle, &allocateInfo, ALLOCATION_CALLBACK, &buffersMemory );
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