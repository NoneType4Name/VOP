#pragma once
#include <platform.hxx>
#include <common/globals.hxx>
#include <common/logging.hxx>
#include <engine.hxx>
#include <surface.hxx>
#include <queue.hxx>

namespace Engine
{
    // struct PhysicalDevice
    // {
    //     PhysicalDevice();
    //     PhysicalDevice( const char *name, VkPhysicalDevice handle );
    //     const char *name { nullptr };
    //     const VkPhysicalDevice getHandle() const;
    //     void setName( const char *name );
    //     void setHandle( VkPhysicalDevice device );

    //   private:
    //     VkPhysicalDevice _handle { nullptr };
    // };

    class device
    {
      private:
        class data;
        std::vector<std::shared_ptr<window::window>> windows;
        deviceID id;
        // renderpassID renderpass_id;
        std::vector<pipelineID> pipelineIDs;
        tools::queueSet _queuesSet;

      public:
        device() = default;
        device( VkPhysicalDevice physicalDevice );
        deviceID getID() const;
        // renderpassID getRenderpassID() const;
        tools::queueSet &getQueueSet() const;
        std::unique_ptr<data> data;
    };

    size_t GetDevicesName( std::unordered_map<deviceID, const char *> &devices, PhysicalDeviceType requeredDeviceType = PhysicalDeviceType::OTHER );

    namespace tools
    {
        const std::vector<VkPhysicalDevice> getDevices();
        void createDevice( VkPhysicalDevice phDevice );
        void destroyDevice();
        const VkDevice getDevice();
        const VkPhysicalDevice getPhysicalDevice();
        queueSet getQueues();
        void getSuitableDevices( std::vector<Device> &devices );
        void getSuitableDevices( std::vector<Device> &devices, uint8_t types );
        void getSuitableDevices( std::vector<PhysicalDevice> &devices );
        void getSuitableDevices( std::vector<PhysicalDevice> &devices, uint8_t types );
        uint32_t memoryTypeIndex( uint32_t type, VkMemoryPropertyFlags properties );
    } // namespace tools
} // namespace Engine
