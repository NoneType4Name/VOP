#include <shader.hxx>
#include <device.hxx>

namespace Engine
{
    namespace tools
    {
        namespace
        {
            shaderID shader_id{ 0 };
            std::unordered_map<shaderID, shader> _shaders{};
            std::unordered_map<const char *, VkShaderModule> _shaderModules{};
        } // namespace

        // void createGrapchiPipeline(){};

        shader::shader( const char *path, const char *mainFuncName, ShaderStage stage ) : id{ ++shader_id }
        {
            info.pName = mainFuncName;
            switch( stage )
            {
                case VERTEX_SHADER_TYPE:
                    info.stage = VK_SHADER_STAGE_VERTEX_BIT;
                    break;
                case FRAGMENT_SHADER_TYPE:
                    info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                default:
                    info.stage = VK_SHADER_STAGE_ALL;
                    break;
            }
            _shaders[ id ] = *this;
        }

        const VkPipelineShaderStageCreateInfo &shader::getInfo() const
        {
            return info;
        }

        const shaderID shader::getID() const
        {
            return id;
        }

        shader::~shader()
        {
            vkDestroyShaderModule( getDevice(), info.module, ALLOCATION_CALLBACK );
        }

        shader &getShader( shaderID id )
        {
            return _shaders[ id ];
        }

        void shader::init()
        {
            if( _shaderModules.count( path ) )
                info.module = _shaderModules[ path ];
            std::ifstream File{ path, std::fstream::ate | std::fstream::binary };
            if( !File.is_open() )
            {
                SPDLOG_CRITICAL( "Failed to open shader: {}.", path );
                throw std::runtime_error( std::format( "Failed to open shader: {}.", path ) );
            }
            size_t shBsize{ static_cast<size_t>( File.tellg() ) };
            std::vector<char> data( shBsize );
            data.resize( shBsize );
            File.seekg( 0 );
            File.read( data.data(), shBsize );
            File.close();
            VkShaderModuleCreateInfo ShaderModuleCreateInfo{};
            ShaderModuleCreateInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            ShaderModuleCreateInfo.codeSize = data.size();
            ShaderModuleCreateInfo.pCode    = reinterpret_cast<const uint32_t *>( data.data() );
            CHECK_RESULT( vkCreateShaderModule( tools::getDevice(), &ShaderModuleCreateInfo, nullptr, &info.module ) );
            _shaderModules[ path ] = info.module;
        }

        void createShaderModules()
        {
            for( auto module : _shaders )
            {
                module.second.init();
            }
        }

        void destroyShaderModules()
        {
            for( auto module : _shaders )
            {
                delete &module.second;
            }
        }

    } // namespace tools
} // namespace Engine