#include <buffer.hxx>
#include <device.hxx>

namespace Engine
{
    namespace tools
    {
        buffer::buffer( VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertiesFlag, VkDeviceSize size, void *data )
        {
            VkBufferCreateInfo BufferCreateInfo{};
            BufferCreateInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            BufferCreateInfo.size        = size;
            BufferCreateInfo.usage       = usageFlags;
            BufferCreateInfo.flags       = BufferCreateInfo.flags ? BufferCreateInfo.flags : 0;
            BufferCreateInfo.sharingMode = BufferCreateInfo.sharingMode ? BufferCreateInfo.sharingMode : VK_SHARING_MODE_EXCLUSIVE;

            CHECK_RESULT( vkCreateBuffer( tools::getDevice(), &BufferCreateInfo, nullptr, &_buffer ) );
            VkMemoryRequirements Requirements;
            vkGetBufferMemoryRequirements( tools::getDevice(), _buffer, &Requirements );
            VkMemoryAllocateInfo MemoryAllocateInfo{};
            MemoryAllocateInfo.sType          = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            MemoryAllocateInfo.allocationSize = Requirements.size;
            VkPhysicalDeviceMemoryProperties memProperties{};
            vkGetPhysicalDeviceMemoryProperties( tools::getPhysicalDevice(), &memProperties );

            for( uint32_t i{ 0 }; i < memProperties.memoryTypeCount; i++ )
            {
                if( ( Requirements.memoryTypeBits & ( 1 << i ) ) && ( ( memProperties.memoryTypes[ i ].propertyFlags & memoryPropertiesFlag ) == memoryPropertiesFlag ) ) MemoryAllocateInfo.memoryTypeIndex = i;
            }
            CHECK_RESULT( vkAllocateMemory( tools::getDevice(), &MemoryAllocateInfo, ALLOCATION_CALLBACK, &_memory ) );
            CHECK_RESULT( vkBindBufferMemory( tools::getDevice(), _buffer, _memory, 0 ) ); // allocate todo?
        }

        buffer::~buffer()
        {
            if( _mapped != nullptr )
                unmap();
            vkFreeMemory( tools::getDevice(), _memory, ALLOCATION_CALLBACK );
            vkDestroyBuffer( tools::getDevice(), _buffer, ALLOCATION_CALLBACK );
        }

        const VkBuffer buffer::getHandle()
        {
            return _buffer;
        }

        const VkDeviceMemory buffer::getMemoryHandle()
        {
            return _memory;
        }

        VkResult buffer::map( VkDeviceSize offset, VkDeviceSize size )
        {
            if( _mapped != nullptr )
                return VK_SUCCESS;
            return vkMapMemory( tools::getDevice(), _memory, offset, size, 0, &_mapped );
        }

        void buffer::copy( void *data, VkDeviceSize size )
        {
            if( _mapped == nullptr )
                return;
            memcpy( _mapped, data, size );
        }

        void buffer::unmap()
        {
            if( _mapped == nullptr )
                return;
            vkUnmapMemory( tools::getDevice(), _memory );
            _mapped = nullptr;
        }
    } // namespace tools
} // namespace Engine