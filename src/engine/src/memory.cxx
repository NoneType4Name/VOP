#include <memory.hxx>

namespace Engine
{
    uint32_t device::DATA_TYPE::setImageMemory( VkImage &image, VkMemoryPropertyFlags properties )
    {
        if ( !images.size() )
            images.resize( description->data->memProperties.memoryTypeCount );
        VkMemoryRequirements mReq {};
        VkMemoryAllocateInfo allocateInfo {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        vkGetImageMemoryRequirements( device, image, &mReq );
        allocateInfo.memoryTypeIndex                          = tools::requeredMemoryTypeIndex( description->data->memProperties, mReq.memoryTypeBits, properties );
        images[ allocateInfo.memoryTypeIndex ].first[ image ] = mReq.size;
        if ( images[ allocateInfo.memoryTypeIndex ].second.second < images[ allocateInfo.memoryTypeIndex ].second.first + mReq.size )
        {
            images[ allocateInfo.memoryTypeIndex ].second.second = allocateInfo.allocationSize = images[ allocateInfo.memoryTypeIndex ].second.first + mReq.size;
            vkFreeMemory( device, imagesMemory, ALLOCATION_CALLBACK );
            vkAllocateMemory( device, &allocateInfo, ALLOCATION_CALLBACK, &imagesMemory );
            images[ allocateInfo.memoryTypeIndex ].second.first = 0;
            for ( const auto img : images[ allocateInfo.memoryTypeIndex ].first )
            {
                vkBindImageMemory( device, img.first, imagesMemory, images[ allocateInfo.memoryTypeIndex ].second.first );
                images[ allocateInfo.memoryTypeIndex ].second.first += img.second;
            }
        }
        else
        {
            vkBindImageMemory( device, image, imagesMemory, images[ allocateInfo.memoryTypeIndex ].second.first );
            images[ allocateInfo.memoryTypeIndex ].second.first += mReq.size;
        }
        return allocateInfo.memoryTypeIndex;
    }
    void device::DATA_TYPE::addImagesMemorySize( uint32_t index, uint32_t size )
    {
        VkMemoryAllocateInfo allocateInfo {};
        allocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.memoryTypeIndex = index;
        // if
        allocateInfo.allocationSize                         = images[ allocateInfo.memoryTypeIndex ].second.second += size;
        images[ allocateInfo.memoryTypeIndex ].second.first = 0;
        vkFreeMemory( device, imagesMemory, ALLOCATION_CALLBACK );
        vkAllocateMemory( device, &allocateInfo, ALLOCATION_CALLBACK, &imagesMemory );
        for ( auto &buf : images[ allocateInfo.memoryTypeIndex ].first )
        {
            vkBindImageMemory( device, buf.first, imagesMemory, images[ allocateInfo.memoryTypeIndex ].second.first );
            images[ allocateInfo.memoryTypeIndex ].second.first += buf.second;
        }
    }

    uint32_t device::DATA_TYPE::setBufferMemory( VkBuffer &buffer, VkMemoryPropertyFlags properties )
    {
        if ( !buffers.size() )
            buffers.resize( description->data->memProperties.memoryTypeCount );
        VkMemoryRequirements mReq {};
        VkMemoryAllocateInfo allocateInfo {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        vkGetBufferMemoryRequirements( device, buffer, &mReq );
        allocateInfo.memoryTypeIndex                            = tools::requeredMemoryTypeIndex( description->data->memProperties, mReq.memoryTypeBits, properties );
        buffers[ allocateInfo.memoryTypeIndex ].first[ buffer ] = mReq.size;
        if ( buffers[ allocateInfo.memoryTypeIndex ].second.second < buffers[ allocateInfo.memoryTypeIndex ].second.first + mReq.size )
        {
            buffers[ allocateInfo.memoryTypeIndex ].second.second = allocateInfo.allocationSize = buffers[ allocateInfo.memoryTypeIndex ].second.first + mReq.size;
            vkFreeMemory( device, buffersMemory, ALLOCATION_CALLBACK );
            vkAllocateMemory( device, &allocateInfo, ALLOCATION_CALLBACK, &buffersMemory );
            buffers[ allocateInfo.memoryTypeIndex ].second.first = 0;
            for ( const auto img : buffers[ allocateInfo.memoryTypeIndex ].first )
            {
                vkBindBufferMemory( device, img.first, buffersMemory, buffers[ allocateInfo.memoryTypeIndex ].second.first );
                buffers[ allocateInfo.memoryTypeIndex ].second.first += img.second;
            }
        }
        else
        {
            vkBindBufferMemory( device, buffer, buffersMemory, buffers[ allocateInfo.memoryTypeIndex ].second.first );
            buffers[ allocateInfo.memoryTypeIndex ].second.first += mReq.size;
        }
        return allocateInfo.memoryTypeIndex;
    }
    void device::DATA_TYPE::resetBufferMemory( VkBuffer &buffer, uint32_t index )
    {
    }
    void device::DATA_TYPE::addBuffersMemorySize( uint32_t index, uint32_t size )
    {
        VkMemoryAllocateInfo allocateInfo {};
        allocateInfo.sType                                   = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.memoryTypeIndex                         = index;
        allocateInfo.allocationSize                          = buffers[ allocateInfo.memoryTypeIndex ].second.second += size;
        buffers[ allocateInfo.memoryTypeIndex ].second.first = 0;
        vkFreeMemory( device, buffersMemory, ALLOCATION_CALLBACK );
        vkAllocateMemory( device, &allocateInfo, ALLOCATION_CALLBACK, &buffersMemory );
        for ( auto &buf : buffers[ allocateInfo.memoryTypeIndex ].first )
        {
            vkBindBufferMemory( device, buf.first, buffersMemory, buffers[ allocateInfo.memoryTypeIndex ].second.first );
            buffers[ allocateInfo.memoryTypeIndex ].second.first += buf.second;
        }
    }
} // namespace Engine