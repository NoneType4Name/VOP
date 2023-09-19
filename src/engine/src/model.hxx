#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <engine.hxx>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

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
            const modelID GetID() const;
            const textureID GetTextureID() const;
            void setVerteciesBufferOffset( const uint64_t offset );
            const uint64_t getVerteciesBufferOffset();
            void setIndeciesBufferOffset( const uint64_t offset );
            const uint64_t getIndeciesBufferOffset();

          private:
            modelID id;
            textureID texture_id;
            std::vector<vertex> vertecies;
            std::vector<uint32_t> indecies;
            uint64_t vertecies_offset{ 0 };
            uint64_t indecies_offset{ 0 };
            // uint64_t texture_coordinate_offset{ 0 };
            // uint64_t normals_offset{ 0 };
        };
        const model &getModel( modelID const id );
    } // namespace tools
} // namespace Engine
