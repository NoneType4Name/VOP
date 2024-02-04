#pragma once
#ifndef IMAGE_HXX
#    define IMAGE_HXX
#    include <common/globals.hxx>
#    include <platform.hxx>
#    include <memory.hxx>

namespace Engine
{
    class image::DATA_TYPE
    {
      public:
        DATA_TYPE() = delete;
        DATA_TYPE( types::image parent, types::device device );
        DATA_TYPE( types::image parent, types::device device, types::image image );
        ~DATA_TYPE();
        types::device device { nullptr };
        device::_memory::allocationAddres addres;
        types::image parentImage { nullptr };
        std::vector<types::image> views;
        types::image parent;
    };
} // namespace Engine
#endif