#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <engine.hxx>

namespace Engine
{
    namespace tools
    {
        struct vertex
        {
            glm::vec3 coordinate;
            glm::vec4 color;
            glm::vec2 texture_coordinate;
            bool operator==( const vertex &other ) const;
        };

        static inline VkVertexInputBindingDescription vertexInputBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 3> vertexInputAttributeDescription();

        class model
        {
          public:
            model() = default;
            model( const char *path );
            const modelID getID() const;
            const textureID getTextureID() const;
            void setTexture( textureID texture );
            void setVerteciesBufferOffset( const uint64_t offset );
            const uint64_t getVerteciesBufferOffset();
            void setIndeciesBufferOffset( const uint64_t offset );
            const uint64_t getIndeciesBufferOffset();
            ~model();

          private:
            modelID id{ 0 };
            textureID texture_id{ 0 };
            std::vector<vertex> vertecies;
            std::vector<uint32_t> indecies;
            uint64_t vertecies_offset{ 0 };
            uint64_t indecies_offset{ 0 };
            // uint64_t texture_coordinate_offset{ 0 };
            // uint64_t normals_offset{ 0 };
        };
        model &getModel( modelID const id );
    } // namespace tools
} // namespace Engine
namespace std
{
    template <>
    struct hash<Engine::tools::vertex>
    {
        size_t operator()( Engine::tools::vertex const &vertex ) const
        {
            return ( hash<glm::vec3>()( vertex.coordinate ) );
        }
    };
} // namespace std