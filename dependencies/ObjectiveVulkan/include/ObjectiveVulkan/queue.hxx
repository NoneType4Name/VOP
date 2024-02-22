#pragma once
#ifndef QUEUE_HXX
#    define QUEUE_HXX
#    include <ObjectiveVulkan/common/globals.hxx>

namespace Engine
{
    struct queue::OBJECTIVE_VULKAN_DATA_TYPE
    {
        OBJECTIVE_VULKAN_DATA_TYPE( types::queue parent, types::device device );
        ~OBJECTIVE_VULKAN_DATA_TYPE();
        std::set<types::commandPool> commandPools;
        types::device device;
        types::queue parent;
    };
} // namespace Engine
#endif