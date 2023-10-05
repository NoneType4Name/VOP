#include <sampler.hxx>
#include <RHI.hxx>
#include <device.hxx>

namespace Engine
{
    namespace tools
    {
        namespace
        {
            std::unordered_map<uint32_t, sampler *> _samplers{};
        }

        sampler::sampler( uint32_t MipLevels )
        {
            mipLevels += MipLevels;
            VkSamplerCreateInfo SamplerCreateInfo{};
            SamplerCreateInfo.sType     = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            SamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
            SamplerCreateInfo.minFilter = VK_FILTER_LINEAR;

            SamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            SamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            SamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;

            SamplerCreateInfo.anisotropyEnable = VK_TRUE;
            SamplerCreateInfo.maxAnisotropy    = static_cast<VkSampleCountFlagBits>( tools::getSettings().settings.MultiSamplingCount );
            SamplerCreateInfo.borderColor      = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

            SamplerCreateInfo.compareEnable = VK_FALSE;
            SamplerCreateInfo.compareOp     = VK_COMPARE_OP_ALWAYS;

            SamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            SamplerCreateInfo.mipLodBias = .0f;
            SamplerCreateInfo.minLod     = .0f;
            SamplerCreateInfo.maxLod     = static_cast<float>( mipLevels );
            _samplers[ mipLevels ]       = this;
            CHECK_RESULT( vkCreateSampler( getDevice(), &SamplerCreateInfo, ALLOCATION_CALLBACK, &handle ) );
        }

        sampler::~sampler()
        {
            vkDestroySampler( getDevice(), handle, ALLOCATION_CALLBACK );
            _samplers.erase( mipLevels );
        }

        uint32_t sampler::getMIPlevels() const
        {
            return mipLevels;
        }

        sampler *getSampler( uint32_t levels )
        {
            return _samplers[ levels ];
        }
    } // namespace tools
} // namespace Engine