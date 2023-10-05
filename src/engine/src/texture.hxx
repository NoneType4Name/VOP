#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <engine.hxx>
#include <image.hxx>
#include <buffer.hxx>

namespace Engine
{
    namespace tools
    {
        class texture
        {
          public:
            texture() = default;
            texture( const char *path );
            ~texture();
            void generateMipMap( commandBuffer &cmdBuff );
            const textureID getID() const;
            uint32_t getMIPlevels();
            const uint8_t *getHandle( const size_t mip_level = 0 ) const;
            const uint32_t getWidth() const;
            const uint32_t getHeight() const;
            const uint32_t getChanels() const;

          private:
            textureID id{ 0 };
            int _x, _y, _c{ 0 };
            std::vector<uint8_t *> mips;
            imageID img{ 0 };
        };

        texture *getTexture( const textureID id );
    } // namespace tools
} // namespace Engine
