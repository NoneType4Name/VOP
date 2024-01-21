#include <shader.hxx>

namespace Engine
{
    namespace tools
    {
        namespace
        {
            std::unordered_map<const char *, VkShaderModule> _shaderModules {};
        } // namespace
        inline VkShaderStageFlagBits userShaderStageToVulkan( ShaderStage stage )
        {
            switch ( stage )
            {
                case VERTEX_SHADER_TYPE:
                    return VK_SHADER_STAGE_VERTEX_BIT;
                case FRAGMENT_SHADER_TYPE:
                    return VK_SHADER_STAGE_FRAGMENT_BIT;
                default:
                    return VK_SHADER_STAGE_ALL;
            }
        }
        VkShaderModule loadShaderModule( VkDevice device, const char *path )
        {
            if ( _shaderModules.count( path ) )
                return _shaderModules[ path ];
            std::ifstream File { path, std::fstream::ate | std::fstream::binary };
            if ( !File.is_open() )
            {
                SPDLOG_CRITICAL( "Failed to open shader: {}.", path );
                throw std::runtime_error( std::format( "Failed to open shader: {}.", path ) );
            }
            size_t shBsize { static_cast<size_t>( File.tellg() ) };
            std::vector<char> data( shBsize );
            data.resize( shBsize );
            File.seekg( 0 );
            File.read( data.data(), shBsize );
            File.close();
            _shaderModules[ path ] = nullptr;
            VkShaderModuleCreateInfo ShaderModuleCreateInfo {};
            ShaderModuleCreateInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            ShaderModuleCreateInfo.codeSize = data.size();
            ShaderModuleCreateInfo.pCode    = reinterpret_cast<const uint32_t *>( data.data() );
            CHECK_RESULT( vkCreateShaderModule( device, &ShaderModuleCreateInfo, nullptr, &_shaderModules[ path ] ) );
            return _shaderModules[ path ];
        }
    } // namespace tools

    // void createGrapchiPipeline(){};
    shader::shader() = default;
    shader::shader( types::device device, const char *path, const char *mainFuncName, ShaderStage stage )
    {
        DEFINE_DATA_FIELD;
        data->device = device;
        data->stage  = tools::userShaderStageToVulkan( stage );
        data->module = tools::loadShaderModule( data->device->handle, path );
        data->pName  = mainFuncName;
    }
    shader::~shader()
    {
        vkDestroyShaderModule( data->device->handle, data->module, ALLOCATION_CALLBACK );
    }

    void InstanceSetup::shader( types::shader shader, VkPipelineShaderStageCreateInfo &stageCreateInfo, void *userPoiner )
    {
        stageCreateInfo.stage  = shader->data->stage;
        stageCreateInfo.module = shader->data->module;
        stageCreateInfo.pName  = shader->data->pName;
    }
} // namespace Engine