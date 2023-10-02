#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <shader.hxx>

namespace Engine
{
    namespace tools
    {
        typedef uint64_t pipelineID;

        class pipeline
        {
          public:
            pipeline() = default;
            pipeline( std::vector<shaderID> );
            const pipelineID getID() const;
            VkPipeline getHandle() const;
            ~pipeline();

          private:
            pipelineID id;
            VkDescriptorPool DescriptorPool{ nullptr };
            VkDescriptorSetLayout DescriptorLayout{ nullptr };
            VkDescriptorSet DescriptorSet{ nullptr };
            VkPipelineLayout PipelineLayout{ nullptr };
            VkPipeline Pipeline{ nullptr };
        };
    } // namespace tools
} // namespace Engine
