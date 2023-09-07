#pragma once
#include <optional>
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
            void setFamilyIndex( uint32_t index );
            void setQueueIndex( uint32_t index );
            const uint32_t GetFamilyIndex() const;
            const uint32_t GetQueueIndex() const;
            const bool inited() const;
            const VkQueue GetHandle() const;
            void operator=( uint32_t right );

          private:
            VkQueue _queue{ nullptr };
            uint32_t _familyIndex{ 0 };
            std::optional<uint32_t> _queueIndex;
        };

        class Queues
        {
          public:
            tools::queue graphic;
            tools::queue present;
            tools::queue transfer;
            // tools::queue compute;

            Queues()  = default;
            ~Queues() = default;
            void operator=( std::initializer_list<uint32_t> right );
            tools::queue operator[]( size_t index );
            void init( VkDevice device );
            const size_t count() const;
        };

        VkDeviceQueueCreateInfo queueCreateInfo( uint32_t index, uint32_t count, const float *priority );
        tools::Queues getIndecies( VkPhysicalDevice device );
    } // namespace tools
} // namespace Engine
