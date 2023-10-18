#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <shader.hxx>
#include <descriptorSet.hxx>

namespace Engine
{
    namespace tools
    {
        void regConstantRange( VkPushConstantRange range );
        class pipeline
        {
          public:
            pipeline() = default;
            pipeline( PipelineInfo createInfo, std::vector<descriptorSet> descriptorID );
            const pipelineID getID() const;
            VkPipeline getHandle() const;
            void init();
            ~pipeline();

          private:
            pipelineID id;
            std::vector<descriptorSet> descriptorSets;
            PipelineInfo info;
            VkPipelineLayout PipelineLayout { nullptr };
            VkPipeline Pipeline { nullptr };
        };
        pipeline *getPipeline( pipelineID );
        void createPipelines();
        void destroyPipelines();
    } // namespace tools
} // namespace Engine
