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
        DATA_TYPE( types::pipeline parent ) :
            parent { parent } {}
        types::device device;
        types::layout layout { nullptr };
        types::pass renderpass { nullptr };
        VkPipeline handle { nullptr };
        types::pipeline parent;

      private:
    };
} // namespace Engine
#endif