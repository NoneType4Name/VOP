#include <GraphicPipeline.hxx>
#include <device.hxx>

namespace Engine
{
    namespace tools
    {
        namespace
        {
            std::unordered_map<const char *, VkShaderModule> _shaderModules{};
        }

        void createGrapchiPipeline(){};

        VkShaderModule
        shaderInfo::load()
        {
            if( _shaderModules.count( path ) )
                return _shaderModules[ path ];
            std::ifstream File{ path, std::fstream::ate | std::fstream::binary };
            if( !File.is_open() )
            {
                SPDLOG_CRITICAL( "Failed to open shader: {}.", path );
                throw std::runtime_error( std::format( "Failed to open shader: {}.", path ) );
            }
            size_t shBsize{ static_cast<size_t>( File.tellg() ) };
            std::vector<char> Data( shBsize );
            File.seekg( 0 );
            File.read( Data.data(), shBsize );
            File.close();
            VkShaderModuleCreateInfo ShaderModuleCreateInfo{};
            ShaderModuleCreateInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            ShaderModuleCreateInfo.codeSize = Data.size();
            ShaderModuleCreateInfo.pCode    = reinterpret_cast<const uint32_t *>( Data.data() );
            VkShaderModule shaderModule{};
            VkResult Result{ vkCreateShaderModule( tools::getDevice(), &ShaderModuleCreateInfo, nullptr, &shaderModule ) };
            _shaderModules[ path ] = shaderModule;
            return shaderModule;
        }
    } // namespace tools
} // namespace Engine