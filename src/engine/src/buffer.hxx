#include <platform.hxx>
#include <common/globals.hxx>
#include <common/logging.hxx>

namespace Engine
{
    namespace tools
    {
        class buffer
        {
          public:
            buffer( VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertiesFlag, VkDeviceSize size, void *data = nullptr );
            ~buffer();
            const VkBuffer getHandle();
            const VkDeviceMemory getMemoryHandle();
            VkResult map( VkDeviceSize offset, VkDeviceSize size );
            void copy( void *data, VkDeviceSize size );
            void unmap();

          private:
            void *_mapped{ nullptr };
            VkDeviceMemory _memory{ nullptr };
            VkBuffer _buffer;
        };
    } // namespace tools
} // namespace Engine