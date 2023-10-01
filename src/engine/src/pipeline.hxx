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
            VkPipeline ptr{ nullptr };
        };
    } // namespace tools
} // namespace Engine
