#pragma once
#ifndef SAMPLER_HXX
#    define SAMPLER_HXX
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <platform.hxx>

namespace Engine
{
    namespace tools
    {
        class sampler
        {
          public:
            sampler() = default;
            sampler( uint32_t mipLevels );
            void init();
            ~sampler();
            VkSampler getHandle() const;

          private:
            uint32_t mipLevels { 1 };
            VkSampler handle { nullptr };
        };

        sampler *getSampler( uint32_t levels );
        void createSamplers();
        void destroySamplers();

    } // namespace tools
} // namespace Engine
#endif