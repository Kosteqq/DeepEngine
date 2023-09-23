#pragma once
#include "Debugs/Logger.h"
#include "VulkanInstance.h"

#include <vulkan/vulkan.h>

namespace DeepEngine::Renderer
{
    class VulkanPhysicalLayer
    {
    public:
        VulkanPhysicalLayer(std::shared_ptr<Core::Debug::Logger> p_logger, VulkanInstance* p_instance);
        
        bool Init();

        const VkPhysicalDevice& GetDevice() const
        { return _physicalDevice; }

    private:
        const VkPhysicalDevice& GetBestDevice(std::vector<VkPhysicalDevice>& p_devices);
        uint32_t RateDevice(const VkPhysicalDevice& p_device);

    private:
        VulkanInstance* _instance = nullptr;
        VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
        
        std::shared_ptr<Core::Debug::Logger> _logger;
    };
}
