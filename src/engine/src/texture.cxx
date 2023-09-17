#include <texture.hxx>
#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <stb_image.h>

namespace Engine
{
    namespace tools
    {
        namespace
        {
            textureID _textureID{ 0 };
        }

        texture::texture( const char *path ) : id{ _textureID++ }
        {
            ptrs.reserve( 1 );
            ptrs.push_back( stbi_load( path, &_x, &_y, &_c, STBI_rgb_alpha ) );
        }

        texture::~texture()
        {
            for( auto p : ptrs )
                stbi_image_free( p );
        }

        const textureID texture::GetID() const
        {
            return id;
        }

        const uint8_t *texture::GetHandle( size_t mip_level ) const
        {
            return ptrs[ mip_level ];
        }

        const uint32_t texture::GetWidth() const
        {
            return static_cast<uint32_t>( _x );
        }

        const uint32_t texture::GetHeight() const
        {
            return static_cast<uint32_t>( _y );
        }

        const uint32_t texture::GetChanels() const
        {
            return static_cast<uint32_t>( _c );
        }

    } // namespace tools

} // namespace Engine
