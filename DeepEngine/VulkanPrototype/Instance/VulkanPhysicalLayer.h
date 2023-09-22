#pragma once
#include "Debugs/Logger.h"
#include "VulkanInstance.h"

#include <vulkan/vulkan.h>

namespace DeepEngine::Renderer
{
    class VulkanPhysicalLayer
    {
    public:
        VulkanPhysicalLayer(const VulkanInstance& p_instance, std::shared_ptr<Core::Debug::Logger> p_logger);
        bool Init();

    private:
        VkPhysicalDevice GetBestDevice(const std::vector<VkPhysicalDevice>& p_devices);
        bool RateDevice(const VkPhysicalDevice& p_device);

    private:
        const VulkanInstance& _instance;
        VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
        
        std::shared_ptr<Core::Debug::Logger> _logger;
    };
}
