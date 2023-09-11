#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>

namespace Engine
{
    namespace tools
    {
        struct shaderInfo
        {
            const char *path{ nullptr };
            const char *mainFuncName{ nullptr };
            VkShaderStageFlagBits stage;
            std::vector<uint32_t> data{};
            VkShaderModule load();
        };
    } // namespace tools
} // namespace Engine