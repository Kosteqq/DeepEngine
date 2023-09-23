#include "VulkanPhysicalLayer.h"

namespace DeepEngine::Renderer
{
    VulkanPhysicalLayer::VulkanPhysicalLayer(std::shared_ptr<Core::Debug::Logger> p_logger, VulkanInstance* p_instance)
        : _logger(p_logger), _instance(p_instance)
    { }

    bool VulkanPhysicalLayer::Init()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(_instance->GetInstance(), &deviceCount, nullptr);
        
        if (deviceCount == 0)
        {
            LOG_ERR(_logger, "Failed to find GPUs with Vulkan support!");
            return false;
        }
        
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(_instance->GetInstance(), &deviceCount, devices.data());
        
        _physicalDevice = GetBestDevice(devices);
        return true;
    }

    const VkPhysicalDevice& VulkanPhysicalLayer::GetBestDevice(std::vector<VkPhysicalDevice>& p_devices)
    {
        uint32_t prevDeviceRate = 0;
        VkPhysicalDevice& bestDevice = p_devices[0];
        
        for (int i = 0; i < p_devices.size(); i++)
        {
            uint32_t rate = RateDevice(p_devices[i]);
            if (rate > prevDeviceRate)
            {
                prevDeviceRate = rate;
                bestDevice = p_devices[i];
            }
        }

        return bestDevice;
    }
    
    uint32_t VulkanPhysicalLayer::RateDevice(const VkPhysicalDevice& p_device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(p_device, &deviceProperties);
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(p_device, &deviceFeatures);

        // Set required features
        // _queueFamilyIndices = GetQueueFamilies(p_device);
            
        return 1;
    }
}
