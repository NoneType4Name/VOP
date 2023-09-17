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
            const textureID GetID() const;
            const uint8_t *GetHandle( size_t mip_level = 0 ) const;
            const uint32_t GetWidth() const;
            const uint32_t GetHeight() const;
            const uint32_t GetChanels() const;

          private:
            textureID id;
            int _x, _y, _c{ 0 };
            std::vector<uint8_t *> ptrs;
        };
    } // namespace tools
} // namespace Engine
