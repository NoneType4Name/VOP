#pragma once
#ifndef IMAGE_HXX
#    define IMAGE_HXX
#    include <common/globals.hxx>

#    include <memory.hxx>

namespace Engine
{
    class image::OBJECTIVE_VULKAN_DATA_TYPE
    {
      public:
        OBJECTIVE_VULKAN_DATA_TYPE() = delete;
        OBJECTIVE_VULKAN_DATA_TYPE( types::image parent, types::device device );
        OBJECTIVE_VULKAN_DATA_TYPE( types::image parent, types::device device, types::image image );
        ~OBJECTIVE_VULKAN_DATA_TYPE();
        types::device device { nullptr };
        device::_memory::allocationAddres addres;
        types::image parentImage { nullptr };
        std::vector<types::image> views;
        types::image parent;
    };
} // namespace Engine
#endif