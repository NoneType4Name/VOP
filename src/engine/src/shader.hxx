#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <engine.hxx>

namespace Engine
{
    namespace tools
    {
        class shader
        {
          public:
            shader() = default;
            shader( const char *path, const char *mainFuncName, ShaderStage stage );
            const VkPipelineShaderStageCreateInfo &getInfo() const;
            const shaderID getID() const;
            void init();
            ~shader();

          private:
            shaderID id;
            VkPipelineShaderStageCreateInfo info{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
            const char *path{ nullptr };
        };

        shader *getShader( shaderID id );
        void createShaderModules();
        void destroyShaderModules();
    } // namespace tools
} // namespace Engine