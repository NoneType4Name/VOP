#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>

namespace Engine
{
    class queueSet;
    class queue
    {
      public:
        queue( queueSet *pSet );
        queue( queueSet *pSet, VkDevice device, uint32_t familyIndex, uint32_t queueIndex, float priority = 0.f );
        queue( queueSet *pSet, uint32_t familyIndex, uint32_t queueIndex, float priority = 0.f );
        void init( VkDevice device );
        void init( VkDevice device, uint32_t familyIndex, uint32_t queueIndex, float priority = 0.f );
        void operator=( std::pair<uint32_t, float> familyPriority );
        void operator=( std::tuple<uint32_t, uint32_t, float> familyIndexPriority );
        bool operator==( const queue &right );
        bool operator!=( const queue &right );
        queueSet *set { nullptr };
        VkQueue handle { nullptr };
        float priority { 0.f };
        uint32_t index { 0 };
        std::optional<uint32_t> familyIndex;
        ~queue() = default;
    };

    class queueSet
    {
      public:
        Engine::queue graphic;
        Engine::queue present;
        Engine::queue transfer;
        // Engine::queue compute { this };
        queueSet();
        queueSet( DeviceDescription *device );
        void init( VkDevice device );
        const size_t count() const;
        void operator=( std::initializer_list<std::pair<uint32_t, float>> familyPriority );
        void operator=( std::initializer_list<std::tuple<uint32_t, uint32_t, float>> familyIndexPriority );
        queue *operator[]( size_t index );
        std::unordered_map<uint32_t, std::pair<std::unordered_map<uint32_t, float *>, std::vector<float>>> &getUniqueIndecies();
        ~queueSet() = default;

      private:
        friend class queue;
        DeviceDescription *description { nullptr };
        // <family<index in family, ptr to priority>, vector of all prioreties in family>
        std::unordered_map<uint32_t, std::pair<std::unordered_map<uint32_t, float *>, std::vector<float>>> _unique;
    };

    VkDeviceQueueCreateInfo queueCreateInfo( uint32_t index, uint32_t count, const float *priority );
    queueSet getIndecies( VkPhysicalDevice device );
} // namespace Engine
