#pragma once
#ifndef QUEUE_HXX
#    define QUEUE_HXX
#    include <common/globals.hxx>

namespace Engine
{
    // typedef std::unordered_map<uint32_t, std::pair<std::unordered_map<uint32_t, float *>, std::vector<float>>> queuesProperties;
    struct queue::DATA_TYPE
    {
        DATA_TYPE( types::queue parent, types::device device );
        ~DATA_TYPE();
        std::set<types::commandPool> commandPools;
        types::device device;
        types::queue parent;
    };

    // struct qProperties
    // {
    //     VkQueueFlags flags;
    //     std::unordered_map<uint32_t, float *> indeciesPriorety;
    //     std::vector<float> prioreties;
    // };

    // typedef std::unordered_map<uint32_t, qProperties> queuesProperties;
    // class queueSet
    // {
    //   public:
    //     Engine::queue graphic;
    //     Engine::queue transfer;
    //     // Engine::queue compute;
    //     Engine::queue present;
    //     queueSet();
    //     queueSet( types::device device );
    //     void init( VkDevice device );
    //     const size_t count() const;
    //     void operator=( std::initializer_list<std::pair<uint32_t, float>> familyPriority );
    //     void operator=( std::initializer_list<std::tuple<uint32_t, uint32_t, float>> familyIndexPriority );
    //     queue *operator[]( size_t index );
    //     queuesProperties &getUniqueIndecies();
    //     ~queueSet();

    //   private:
    //     // <family<index in family, ptr to priority>, vector of all prioreties in family>
    //     queuesProperties _unique;
    // };
} // namespace Engine
#endif