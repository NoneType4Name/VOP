#include <device.hxx>
#include <RHI.hxx>

namespace Engine
{
    namespace tools
    {
        namespace
        {
            std::vector<VkExtensionProperties> _extensions;
        }
        void createDevice( VkPhysicalDevice phDevice );
        void destroyDevice();
        void getDevice();

    } // namespace tools
} // namespace Engine
