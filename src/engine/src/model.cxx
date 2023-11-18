#include <model.hxx>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <pipeline.hxx>
#include <buffer.hxx>
#include <texture.hxx>
#include <sampler.hxx>

namespace Engine
{
    struct DefaultDescriptorData
    {
        buffer UniformObjectBuffer;
    };

    struct DefaultUniformObject
    {
        glm::mat4 model;
    };

    bool vertex::operator==( const vertex &other ) const
    {
        return coordinate == other.coordinate && color == other.color && texture_coordinate == other.texture_coordinate;
    }

    VkVertexInputBindingDescription vertexInputBindingDescription()
    {
        return { 0, sizeof( vertex ), VK_VERTEX_INPUT_RATE_VERTEX };
    }

    std::array<VkVertexInputAttributeDescription, 3> vertexInputAttributeDescription()
    {
        std::array<VkVertexInputAttributeDescription, 3> VertexInputAttributeDescription {};
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

    model::model( const char *path )
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;
        if ( !tinyobj::LoadObj( &attrib, &shapes, &materials, &warn, &err, path ) )
        {
            SPDLOG_CRITICAL( "Failed to Load model {}:\nwarning:\t{}\nerror:\t{}.", path, warn, err );
            assert( 1 );
        }
        if ( warn.length() )
            SPDLOG_WARN( "Warn with load model {}:{}", path, warn );
        if ( err.length() )
            SPDLOG_ERROR( "Error with load model {}:{}", path, err );
        std::unordered_map<vertex, uint64_t> uniqueVertices {};

        for ( const auto &shape : shapes )
        {
            for ( const auto &index : shape.mesh.indices )
            {
                vertex _vert {};

                _vert.coordinate = {
                    attrib.vertices[ 3 * index.vertex_index + 0 ],
                    attrib.vertices[ 3 * index.vertex_index + 1 ],
                    attrib.vertices[ 3 * index.vertex_index + 2 ] };

                _vert.texture_coordinate = {
                    attrib.texcoords[ 2 * index.texcoord_index + 0 ],
                    1.0f - attrib.texcoords[ 2 * index.texcoord_index + 1 ] };

                _vert.color = { 1.0f, 1.0f, 1.0f, 1.f };
                if ( !uniqueVertices.count( _vert ) )
                {
                    uniqueVertices[ _vert ] = vertecies.size();
                    vertecies.push_back( _vert ); // todo
                }
                indecies.push_back( uniqueVertices[ _vert ] );
            }
        }
        // regConstantRange( { VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof( PushConstantLayout ) } );
    }

    texture *model::getTexture() const
    {
        return texture;
    }

    Engine::texture *model::setTexture( Engine::texture *texture )
    {
        std::swap( this->texture, texture );
        this->texture = texture;
        return texture;
    }

    void model::setVerteciesBufferOffset( const uint64_t offset )
    {
        vertecies_offset = offset;
    }

    uint64_t model::getVerteciesBufferOffset()
    {
        return vertecies_offset;
    }

    void model::setIndeciesBufferOffset( const uint64_t offset )
    {
        indecies_offset = offset;
    }

    uint64_t model::getIndeciesBufferOffset()
    {
        return indecies_offset;
    }

    // void model::setupDescriptorSet()
    // {
    //     descriptorSet_id = ( new tools::descriptorSet {
    //                              { { 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_ALL_GRAPHICS },
    //                                { 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT } } } )
    //                            ->getID();
    // }

    void model::init()
    {
        DefaultDescriptorData *pDescriptorSet { static_cast<DefaultDescriptorData *>( descriptorSetStructure ) };
        pDescriptorSet->UniformObjectBuffer.init( VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof( DefaultDescriptorData ) );

        VkDescriptorBufferInfo DescriptorBufferInfo {};
        DescriptorBufferInfo.buffer = pDescriptorSet->UniformObjectBuffer.getHandle();
        DescriptorBufferInfo.offset = 0;
        DescriptorBufferInfo.range  = sizeof( DefaultDescriptorData );

        VkWriteDescriptorSet WriteUBDescriptorSet {};
        WriteUBDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        WriteUBDescriptorSet.dstSet          = getDescriptorSet( descriptorSet_id )->getHandle();
        WriteUBDescriptorSet.dstBinding      = 0;
        WriteUBDescriptorSet.dstArrayElement = 0;
        WriteUBDescriptorSet.descriptorCount = 1;
        WriteUBDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        WriteUBDescriptorSet.pBufferInfo     = &DescriptorBufferInfo;

        VkDescriptorImageInfo DescriptorImageInfo {};
        DescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        DescriptorImageInfo.imageView   = getImage( getTexture( texture_id )->getImageID() )->getImageView();  // TODO: As normal class method
        DescriptorImageInfo.sampler     = getSampler( getTexture( texture_id )->getMIPlevels() )->getHandle(); // TODO: As normal class method

        VkWriteDescriptorSet WriteSamplerDescriptorSet {};
        WriteSamplerDescriptorSet.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        WriteSamplerDescriptorSet.dstSet          = getDescriptorSet( descriptorSet_id )->getHandle();
        WriteSamplerDescriptorSet.dstBinding      = 1;
        WriteSamplerDescriptorSet.dstArrayElement = 0;
        WriteSamplerDescriptorSet.descriptorCount = 1;
        WriteSamplerDescriptorSet.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        WriteSamplerDescriptorSet.pImageInfo      = &DescriptorImageInfo;
    }

    model::~model()
    {
        _models[ id ] = nullptr;
    }

    model *getModel( const modelID id )
    {
        return _models[ id ];
    }
} // namespace Engine
