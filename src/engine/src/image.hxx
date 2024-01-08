#pragma once
#ifndef IMAGE_HXX
#    define IMAGE_HXX
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <platform.hxx>
#    include <memory.hxx>

namespace Engine
{
    class image::DATA_TYPE
    {
      public:
        DATA_TYPE() = delete;
        DATA_TYPE( types::image parent, types::device device, VkImageCreateInfo ImageCreateInfo, VkImageViewCreateInfo ImageViewCreateInfo );
        DATA_TYPE( types::image parent, types::device device, types::image image, VkImageViewCreateInfo ImageViewCreateInfo );
        ~DATA_TYPE();
        types::device device { nullptr };
        VkImageCreateInfo ImageInfo {};
        VkImageViewCreateInfo ImageViewInfo {};
        VkImageView view { nullptr };
        memory::allocationAddres addres;
        types::image image { nullptr };
        std::vector<types::image> views;
        types::image parent;
    };
} // namespace Engine
#endif