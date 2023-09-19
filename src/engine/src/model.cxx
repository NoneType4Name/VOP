#include <model.hxx>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace Engine
{
    namespace tools
    {
        namespace
        {
            modelID _modelID{ 1 };
            std::unordered_map<modelID, model> _models;

        } // namespace

        bool vertex::operator==( const vertex &other ) const
        {
            return coordinate == other.coordinate && color == other.color && texture_coordinate == other.texture_coordinate;
        }

        static inline VkVertexInputBindingDescription vertexInputBindingDescription()
        {
            return { 0, sizeof( vertex ), VK_VERTEX_INPUT_RATE_VERTEX };
        }

        static std::array<VkVertexInputAttributeDescription, 3> vertexInputAttributeDescription()
        {
            std::array<VkVertexInputAttributeDescription, 3> VertexInputAttributeDescription{};
            VertexInputAttributeDescription[ 0 ].binding  = 0;
            VertexInputAttributeDescription[ 0 ].location = 0;
            VertexInputAttributeDescription[ 0 ].format   = VK_FORMAT_R32G32B32_SFLOAT;
            VertexInputAttributeDescription[ 0 ].offset   = offsetof( vertex, coordinate );

            VertexInputAttributeDescription[ 1 ].binding  = 0;
            VertexInputAttributeDescription[ 1 ].location = 1;
            VertexInputAttributeDescription[ 1 ].format   = VK_FORMAT_R32G32B32A32_SFLOAT;
            VertexInputAttributeDescription[ 1 ].offset   = offsetof( vertex, color );

            VertexInputAttributeDescription[ 2 ].binding  = 0;
            VertexInputAttributeDescription[ 2 ].location = 2;
            VertexInputAttributeDescription[ 2 ].format   = VK_FORMAT_R32G32_SFLOAT;
            VertexInputAttributeDescription[ 2 ].offset   = offsetof( vertex, texture_coordinate );
            return VertexInputAttributeDescription;
        }

        model::model( const char *path ) : id{ _modelID++ }
        {
            tinyobj::attrib_t attrib;
            std::vector<tinyobj::shape_t> shapes;
            std::vector<tinyobj::material_t> materials;
            std::string warn, err;
            if( !tinyobj::LoadObj( &attrib, &shapes, &materials, &warn, &err, path ) )
            {
                SPDLOG_CRITICAL( "Failed to Load model {}:\nwarning:\t{}\nerror:\t{}.", path, warn, err );
                assert( 1 );
            }
            if( warn.length() )
                SPDLOG_WARN( "Warn with load model {}:{}", path, warn );
            if( err.length() )
                SPDLOG_ERROR( "Error with load model {}:{}", path, err );

            for( const auto &shape : shapes )
            {
                for( const auto &index : shape.mesh.indices )
                {
                    vertex vertex{};

                    vertex.coordinate = {
                        attrib.vertices[ 3 * index.vertex_index + 0 ],
                        attrib.vertices[ 3 * index.vertex_index + 1 ],
                        attrib.vertices[ 3 * index.vertex_index + 2 ] };

                    vertex.texture_coordinate = {
                        attrib.texcoords[ 2 * index.texcoord_index + 0 ],
                        1.0f - attrib.texcoords[ 2 * index.texcoord_index + 1 ] };

                    vertex.color = { 1.0f, 1.0f, 1.0f, 1.f };

                    // indecies.push_back( index.vertex_index );
                    // vertecies.push_back( vertex ); // todo
                }
            }

            _models[ id ] = *this;
        }

        const modelID model::GetID() const
        {
            return id;
        }

        const textureID model::GetTextureID() const
        {
            return texture_id;
        }

        void model::setVerteciesBufferOffset( const uint64_t offset )
        {
            vertecies_offset = offset;
        }

        const uint64_t model::getVerteciesBufferOffset()
        {
            return vertecies_offset;
        }

        void model::setIndeciesBufferOffset( const uint64_t offset )
        {
            indecies_offset = offset;
        }

        const uint64_t model::getIndeciesBufferOffset()
        {
            return indecies_offset;
        }

        const model &getTexture( const modelID id )
        {
            return _models[ id ];
        }
    } // namespace tools
} // namespace Engine

namespace std
{
    template <>
    struct hash<Engine::tools::vertex>
    {
        size_t operator()( Engine::tools::vertex const &vertex ) const
        {
            return ( ( hash<glm::vec3>()( vertex.coordinate ) ^ ( hash<glm::vec3>()( vertex.color ) << 1 ) ) >> 1 ) ^ ( hash<glm::vec2>()( vertex.texture_coordinate ) << 1 );
        }
    };
} // namespace std
