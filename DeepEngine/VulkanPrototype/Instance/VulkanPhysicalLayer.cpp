#include "VulkanPhysicalLayer.h"

namespace DeepEngine::Renderer
{
    
    VulkanPhysicalLayer::VulkanPhysicalLayer(const VulkanInstance& p_instance,
        std::shared_ptr<Core::Debug::Logger> p_logger)
        : _instance(p_instance), _logger(p_logger)
    {
            
    }

    bool VulkanPhysicalLayer::Init()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
        
        if (deviceCount == 0)
        {
            LOG_ERR(_logger, "Failed to find GPUs with Vulkan support!");
            return false;
        }
        
        std::vector<VkPhysicalDevice> devices;
        vkEnumeratePhysicalDevices(_instance.GetInstance(), &deviceCount, devices.data());
        
        _physicalDevice = GetBestDevice(devices);
        return true;
    }
    
    VkPhysicalDevice VulkanPhysicalLayer::GetBestDevice(const std::vector<VkPhysicalDevice>& p_devices)
    {
        for (int i = 0; i < p_devices.size(); i++)
        {
            if (RateDevice(p_devices[i]))
            {
                return p_devices[i];
            }
        }

        LOG_ERR(_logger, "Failed to find any suitable GPU!");
        return VK_NULL_HANDLE;
    }
    
    bool VulkanPhysicalLayer::RateDevice(const VkPhysicalDevice& p_device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(p_device, &deviceProperties);
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(p_device, &deviceFeatures);

        // Set required features
        // _queueFamilyIndices = GetQueueFamilies(p_device);
            
        return true;
    }
}
