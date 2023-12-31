#pragma once
#ifndef QUEUE_HXX
#    define QUEUE_HXX
#    include <common/globals.hxx>
#    include <common/logging.hxx>
#    include <platform.hxx>

namespace Engine
{
    // typedef std::unordered_map<uint32_t, std::pair<std::unordered_map<uint32_t, float *>, std::vector<float>>> queuesProperties;
    struct qProperties
    {
        VkQueueFlags flags;
        std::unordered_map<uint32_t, float *> indeciesPriorety;
        std::vector<float> prioreties;
    };
    typedef std::unordered_map<uint32_t, qProperties> queuesProperties;
    class queueSet;
    class queue
    {
      public:
        queue( queueSet *pSet );
        queue( queueSet *pSet, VkDevice device, uint32_t familyIndex, uint32_t queueIndex, VkQueueFlags flags = 0, float priority = 0.f );
        queue( queueSet *pSet, uint32_t familyIndex, uint32_t queueIndex, VkQueueFlags flags = 0, float priority = 0.f );
        void init( VkDevice device );
        void init( VkDevice device, uint32_t familyIndex, uint32_t queueIndex, float priority = 0.f );
        void operator=( std::pair<uint32_t, float> familyPriority );
        void operator=( std::tuple<uint32_t, uint32_t, float> familyIndexPriority );
        bool operator==( const queue &right );
        bool operator!=( const queue &right );
        queueSet *set { nullptr };
        VkQueue handle { nullptr };
        VkQueueFlags flags { 0 };
        float priority { 1.f };
        uint32_t index { 0 };
        std::optional<uint32_t> familyIndex;
        ~queue() = default;
    };

    class queueSet
    {
      public:
        Engine::queue graphic;
        Engine::queue transfer;
        // Engine::queue compute;
        Engine::queue present;
        queueSet();
        queueSet( types::device parent );
        void init( VkDevice device );
        const size_t count() const;
        void operator=( std::initializer_list<std::pair<uint32_t, float>> familyPriority );
        void operator=( std::initializer_list<std::tuple<uint32_t, uint32_t, float>> familyIndexPriority );
        queue *operator[]( size_t index );
        queuesProperties &getUniqueIndecies();
        ~queueSet();

      private:
        friend class queue;
        types::device parent;
        // <family<index in family, ptr to priority>, vector of all prioreties in family>
        queuesProperties _unique;
    };

    VkDeviceQueueCreateInfo queueCreateInfo( uint32_t index, uint32_t count, const float *priority );
    queueSet getIndecies( VkPhysicalDevice device );
} // namespace Engine
#endif