#pragma once
#include <platform.hxx>
#include <common/globals.hxx>
#include <common/logging.hxx>

namespace Engine
{
    namespace tools
    {
        void createDevice( VkPhysicalDevice phDevice );
        void destroyDevice();
        const VkDevice getDevice();
    } // namespace tools
} // namespace Engine
