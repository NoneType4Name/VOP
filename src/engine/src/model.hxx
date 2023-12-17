#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <descriptorPool.hxx>
#include <engine.hxx>

namespace Engine
{
    struct pushConstant
    {
        glm::mat4 tranfromation;
    };

    struct vertex
    {
        glm::vec3 coordinate;
        glm::vec4 color;
        glm::vec2 texture_coordinate;
        bool operator==( const vertex &other ) const;
    };

    class model::DATA_TYPE
    {
      public:
        struct descriptorSetLayoutInfo
        {
            VkDescriptorType descriptorType;
            const void *pNext;
            uint32_t binding;
            uint32_t descriptorCount;
            VkShaderStageFlags stageFlags;
            uint32_t dstArrayElement;
            const VkDescriptorImageInfo *pImageInfo;
            const VkDescriptorBufferInfo *pBufferInfo;
            const VkSampler *pImmutableSamplers;
            const VkBufferView *pTexelBufferView;
        };
        void *descriptorSetStructure { nullptr };
        Engine::texture *texture { nullptr };
        // std::vector<vertex> vertecies;
        // std::vector<uint32_t> indecies;
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