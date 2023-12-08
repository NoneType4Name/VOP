#pragma once
#ifndef DESCRIPTOR_POOL_HXX
#    define DESCRIPTOR_POOL_HXX
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <platform.hxx>

namespace Engine
{
    struct descriptorPool::descriptorSetLayoutInfo
    {
        VkDescriptorType descriptorType;
        const void *pNext;
        uint32_t binding;
        uint32_t descriptorCount;
        VkShaderStageFlags stageFlags;
        uint32_t dstArrayElement;
        const VkDescriptorImageInfo *pImageInfo;
        const VkDescriptorBufferInfo *pBufferInfo;
        const VkSampler *pImmutableSamplers;
        const VkBufferView *pTexelBufferView;
    };

    struct descriptorPool::DATA_TYPE
    {
        DATA_TYPE( descriptorPool *parent ) :
            parent { parent } {}
        std::vector<VkDescriptorSetLayout> layouts;
        std::vector<VkDescriptorSet> sets;
        types::device device { nullptr };
        VkDescriptorPool handle { nullptr };
        void *userData { nullptr };
        descriptorPool *parent { nullptr };
    };
} // namespace Engine
#endif