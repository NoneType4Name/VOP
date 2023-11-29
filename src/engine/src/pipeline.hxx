#pragma once
#ifndef PIPELINE_HXX
#    define PIPELINE_HXX
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <platform.hxx>
#    include <shader.hxx>
#    include <descriptorPool.hxx>

namespace Engine
{
    // void regConstantRange( VkPushConstantRange range );

    struct pipeline::DATA_TYPE
    {
      public:
        VkPipelineLayout PipelineLayout { nullptr };
        VkPipeline Pipeline { nullptr };

      private:
    };
} // namespace Engine
#endif