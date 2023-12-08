#pragma once
#ifndef SHADER_HXX
#    define SHADER_HXX
#    include <common/globals.hxx>
#    include <common/logging.hxx>
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
        DATA_TYPE( shader *parent ) :
            parent { parent } {}
        types::device device { nullptr };
        VkShaderModule module { nullptr };
        const char *pName { nullptr };
        VkShaderStageFlagBits stage;
        shader *parent { nullptr };
    };

    struct layout::DATA_TYPE
    {
        types::device device { nullptr };
        VkPipelineLayout handle { nullptr };
        types::descriptorPool pool { nullptr };
    };
} // namespace Engine
#endif