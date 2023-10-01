#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <engine.hxx>

namespace Engine
{
    namespace tools
    {
        typedef uint64_t shaderID;
        class shader
        {
          public:
            shader() = default;
            shader( const char *path, const char *mainFuncName, ShaderStage stage );
            const VkPipelineShaderStageCreateInfo &getInfo() const;
            const shaderID getID() const;
            ~shader();

          private:
            shaderID id;
            VkPipelineShaderStageCreateInfo info{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
        };

        shader &getShader( shaderID id );
    } // namespace tools
} // namespace Engine