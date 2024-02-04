#pragma once
#ifndef SHADER_HXX
#    define SHADER_HXX
#    include <common/globals.hxx>
#    include <platform.hxx>
#    include <EHI.hxx>

namespace Engine
{
    namespace tools
    {
        VkShaderModule loadShaderModule( VkDevice device, const char *path );
        inline VkShaderStageFlagBits userShaderStageToVulkan( ShaderStage stage );
    } // namespace tools

    struct shader::DATA_TYPE
    {
        DATA_TYPE( types::shader parent ) :
            parent { parent } {}
        types::device device { nullptr };
        VkShaderModule module { nullptr };
        const char *pName { nullptr };
        VkShaderStageFlagBits stage;
        types::shader parent;
    };

    struct layout::DATA_TYPE
    {
        types::device device { nullptr };
        VkPipelineLayout handle { nullptr };
        types::descriptorPool pool { nullptr };
        types::layout parent;
    };
} // namespace Engine
#endif