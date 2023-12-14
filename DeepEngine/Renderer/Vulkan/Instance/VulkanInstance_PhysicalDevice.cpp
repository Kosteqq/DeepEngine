#include "VulkanInstance.h"

namespace DeepEngine::Renderer::Vulkan
{
    bool VulkanInstance::OnInitializePhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
        
        if (deviceCount == 0)
        {
            VULKAN_ERR("Failed to find any GPU with Vulkan support!");
            return false;
        }
        
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());
        
        if (!FindMatchingPhysicalDevice(devices))
        {
            VULKAN_ERR("Failed to find any physical device with matched extensions");
            return false;
        }

        // Debug pint out all extensions
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extensionCount, nullptr);
        auto availableExtensions = std::vector<VkExtensionProperties>(extensionCount);
        vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extensionCount, availableExtensions.data());

        VULKAN_INFO("Physical Device info:\nDevice Name: {}\nDevice Type: {}\nDriver Version: {}\nAPI Version: {}\n",
            _physicalDeviceProperties.deviceName,
            _physicalDeviceProperties.apiVersion,
            string_VkPhysicalDeviceType(_physicalDeviceProperties.deviceType),
            _physicalDeviceProperties.driverVersion);
        
        VULKAN_TRACE("Found Vulkan physical device (with {} extensions)", availableExtensions.size());
        for (int i = 0; i < availableExtensions.size(); i++)
        {
            VULKAN_TRACE("\t{:<45} (v.{})",
                availableExtensions[i].extensionName,
                availableExtensions[i].specVersion);
        }
        
        return true;
    }
    
    inline void VulkanInstance::EnablePhysicalExtension(const VkExtensionProperties& p_extension)
    { EnablePhysicalExtension(p_extension.extensionName); }
    
    void VulkanInstance::EnablePhysicalExtension(const std::string& p_extensionName)
    {
        _enabledPhysicalExtensionNames.push_back(p_extensionName);
    }

    bool VulkanInstance::IsPhysicalExtensionEnabled(const std::string& p_extensionName) const
    {
        for (int i = 0; i < _enabledPhysicalExtensionNames.size(); i++)
        {
            if (_enabledPhysicalExtensionNames[i] == p_extensionName)
            {
                return true;
            }
        }
        
        return false;
    }


    bool VulkanInstance::FindMatchingPhysicalDevice(const std::vector<VkPhysicalDevice>& p_devices)
    {
        for (uint32_t i = 0; i < p_devices.size(); i++)
        {
            const VkPhysicalDevice device = p_devices[i];

            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
            std::vector<VkExtensionProperties> deviceExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, deviceExtensions.data());

            uint32_t foundExtensionsCount = 0;
            for (uint32_t j = 0; j < _enabledPhysicalExtensionNames.size(); j++)
            {
                for (uint32_t k = 0; k < deviceExtensions.size(); k++)
                {
                    if (_enabledPhysicalExtensionNames[j] == std::string(deviceExtensions[k].extensionName))
                    {
                        foundExtensionsCount++;
                        break;
                    }
                }
            }

            if (_enabledPhysicalExtensionNames.size() == foundExtensionsCount)
            {
                _physicalDevice = device;
                vkGetPhysicalDeviceProperties(_physicalDevice, &_physicalDeviceProperties);
                vkGetPhysicalDeviceFeatures(_physicalDevice, &_physicalDeviceFeatures);
                return true;
            }
        }

        return false;
    }
}
