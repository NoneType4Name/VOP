#pragma once
#include <engine.hxx>
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>

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
            const textureID getID() const;
            const uint8_t *getHandle( const size_t mip_level = 0 ) const;
            const uint32_t getWidth() const;
            const uint32_t getHeight() const;
            const uint32_t getChanels() const;

          private:
            textureID id{ 0 };
            int _x, _y, _c{ 0 };
            std::vector<uint8_t *> mips;
        };

        const texture &getTexture( const textureID id );
    } // namespace tools
} // namespace Engine
