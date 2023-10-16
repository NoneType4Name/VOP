#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>

namespace Engine
{
    namespace tools
    {
        class sampler
        {
          public:
            sampler() = default;
            sampler( uint32_t mipLevels );
            ~sampler();
            uint32_t getMIPlevels() const;

          private:
            uint32_t mipLevels { 1 };
            VkSampler handle { nullptr };
        };

        sampler *getSampler( uint32_t levels );
    } // namespace tools
} // namespace Engine