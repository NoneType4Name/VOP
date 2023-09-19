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
            textureID _textureID{ 1 };
            std::unordered_map<textureID, texture> _textures;
        } // namespace

        texture::texture( const char *path ) : id{ _textureID++ }
        {
            mips.reserve( 1 );
            mips.push_back( stbi_load( path, &_x, &_y, &_c, STBI_rgb_alpha ) );
            _textures[ id ] = *this;
        }

        texture::~texture()
        {
            for( auto p : mips )
                stbi_image_free( p );
        }

        const textureID texture::GetID() const
        {
            return id;
        }

        const uint8_t *texture::GetHandle( const size_t mip_level ) const
        {
            return mips[ mip_level ];
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

        const texture &getTexture( const textureID id )
        {
            return _textures[ id ];
        }

    } // namespace tools
} // namespace Engine
