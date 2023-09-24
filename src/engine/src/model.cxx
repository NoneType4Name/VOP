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
            std::unordered_map<vertex, uint64_t> uniqueVertices{};

            for( const auto &shape : shapes )
            {
                for( const auto &index : shape.mesh.indices )
                {
                    vertex _vert{};

                    _vert.coordinate = {
                        attrib.vertices[ 3 * index.vertex_index + 0 ],
                        attrib.vertices[ 3 * index.vertex_index + 1 ],
                        attrib.vertices[ 3 * index.vertex_index + 2 ] };

                    _vert.texture_coordinate = {
                        attrib.texcoords[ 2 * index.texcoord_index + 0 ],
                        1.0f - attrib.texcoords[ 2 * index.texcoord_index + 1 ] };

                    _vert.color = { 1.0f, 1.0f, 1.0f, 1.f };
                    if( !uniqueVertices.count( _vert ) )
                    {
                        uniqueVertices[ _vert ] = vertecies.size();
                        vertecies.push_back( _vert ); // todo}
                    }
                    indecies.push_back( uniqueVertices[ _vert ] );
                }
            }

            _models[ id ] = *this;
        }

        const modelID model::getID() const
        {
            return id;
        }

        const textureID model::getTextureID() const
        {
            return texture_id;
        }

        void model::setTexture( textureID texture )
        {
            texture_id = texture;
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

        model &getTexture( const modelID id )
        {
            return _models[ id ];
        }
    } // namespace tools
} // namespace Engine
