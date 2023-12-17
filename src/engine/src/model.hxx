#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>
#include <descriptorPool.hxx>
#include <engine.hxx>

namespace Engine
{
    VkVertexInputBindingDescription vertexInputBindingDescription();
    std::array<VkVertexInputAttributeDescription, 3> vertexInputAttributeDescription();
    struct PushConstant
    {
        glm::mat4 tranfromation;
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
        // descriptorSet *descriptorSet_id { nullptr };
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