#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <engine.hxx>
#include <image.hxx>
#include <descriptorSet.hxx>
#include <buffer.hxx>

namespace Engine
{
    class texture
    {
      public:
        texture() = default;
        texture( const char *path );
        ~texture();
        virtual void generateMipMap( commandBuffer &cmdBuff );
        void init();
        const textureID getID() const;
        const descriptorSetID getDescriptorSetID() const;
        uint32_t getMIPlevels();
        const uint8_t *getData( const size_t mip_level = 0 ) const;
        imageID getImageID() const;
        const uint32_t getWidth() const;
        const uint32_t getHeight() const;
        const uint32_t getChanels() const;

      private:
        void *descriptorSetStructure { nullptr };
        textureID id { 0 };
        descriptorSetID DescriptorSetID { 0 };
        int _x, _y, _c { 0 };
        std::vector<uint8_t *> mips;
        imageID img { 0 };
    };
} // namespace Engine
