#pragma once
#ifndef PIPELINE_HXX
#    define PIPELINE_HXX
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <platform.hxx>
#    include <shader.hxx>
#    include <descriptorPool.hxx>
#    include <EHI.hxx>

namespace Engine
{
    struct pipeline::DATA_TYPE
    {
      public:
        types::device device { nullptr };
        types::layout layout { nullptr };
        types::pass renderpass { nullptr };
        VkPipeline handle { nullptr };

      private:
    };
} // namespace Engine
#endif