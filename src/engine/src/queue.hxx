#pragma once
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <platform.hxx>

namespace Engine
{
    namespace tools
    {
        class queue
        {
          public:
            queue() = default;
            queue( VkDevice device, uint32_t familyIndex, uint32_t queueIndex );
            ~queue() = default;
            void init( VkDevice device, uint32_t familyIndex, uint32_t queueIndex );
            void init( VkDevice device );
            void setHandle( VkQueue queue );
            void setFamilyIndex( uint32_t index );
            void setQueueIndex( uint32_t index );
            const VkQueue getHandle() const;
            const uint32_t getFamilyIndex() const;
            const uint32_t getQueueIndex() const;
            const bool inited() const;
            void operator=( uint32_t right );

          private:
            VkQueue _queue { nullptr };
            uint32_t _queueIndex { 0 };
            std::optional<uint32_t> _familyIndex;
        };

        class queueSet
        {
          public:
            tools::queue graphic;
            tools::queue present;
            tools::queue transfer;
            // tools::queue compute;

            queueSet() = default;
            void init( VkDevice device );
            const size_t count() const;
            void operator=( std::initializer_list<uint32_t> right );
            queue operator[]( size_t index );
            std::unordered_map<uint32_t, std::pair<uint32_t, std::vector<float>>> &getUniqueIndeciesCount();
            ~queueSet() = default;

          private:
            std::unordered_map<uint32_t, std::pair<uint32_t, std::vector<float>>> _unique {};
        };

        VkDeviceQueueCreateInfo queueCreateInfo( uint32_t index, uint32_t count, const float *priority );
        queueSet getIndecies( VkPhysicalDevice device );
    } // namespace tools
} // namespace Engine
