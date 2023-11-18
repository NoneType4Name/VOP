#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <descriptorSet.hxx>
#include <engine.hxx>

namespace Engine
{

    struct vertex
    {
        glm::vec3 coordinate;
        glm::vec4 color;
        glm::vec2 texture_coordinate;
        bool operator==( const vertex &other ) const;
    };

    VkVertexInputBindingDescription vertexInputBindingDescription();
    std::array<VkVertexInputAttributeDescription, 3> vertexInputAttributeDescription();
    struct PushConstantLayout
    {
        glm::mat4 tranfromation;
    };

    class model
    {
      public:
        model() = default;
        model( const char *path );
        Engine::texture *getTexture() const;
        Engine::texture *setTexture( Engine::texture *texture );
        void setVerteciesBufferOffset( const uint64_t offset );
        uint64_t getVerteciesBufferOffset();
        void setIndeciesBufferOffset( const uint64_t offset );
        uint64_t getIndeciesBufferOffset();
        void init();
        void setPosition( glm::vec3 positionVector ); // todo
        void setScale( glm::mat4 scaleMatrix );       // todo
        void setRotation( glm::mat4 rotateMatrix );   // todo
        // virtual void setupDescriptorSet();
        ~model();

      private:
        // descriptorSet *descriptorSet_id { nullptr };
        void *descriptorSetStructure { nullptr };
        Engine::texture *texture { nullptr };
        std::vector<vertex> vertecies;
        std::vector<uint32_t> indecies;
        uint64_t vertecies_offset { 0 };
        uint64_t indecies_offset { 0 };
        glm::vec3 position;
        glm::mat4 rotation;
        glm::mat4 scale;
    };

} // namespace Engine
namespace std
{
    template<>
    struct hash<Engine::vertex>
    {
        size_t operator()( Engine::vertex const &vertex ) const
        {
            return ( hash<glm::vec3>()( vertex.coordinate ) );
        }
    };
} // namespace std