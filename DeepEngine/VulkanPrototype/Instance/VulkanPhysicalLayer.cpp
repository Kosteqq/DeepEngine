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
        
        if (!GetBestDevice(devices))
        {
            LOG_ERR(_logger, "Failed to find any physical device with matched extensions");
            return false;
        }

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extensionCount, nullptr);
        _availableExtensions = std::vector<VkExtensionProperties>(extensionCount);
        vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &extensionCount, _availableExtensions.data());
        
        LOG_DEBUG(_logger, "Found Vulkan physical device (with {} extensions)", _availableExtensions.size());
        for (int i = 0; i < _availableExtensions.size(); i++)
        {
            LOG_DEBUG(_logger, "{:<45} (v.{}): [{}]",
                _availableExtensions[i].extensionName,
                _availableExtensions[i].specVersion,
                IsExtensionsEnabled(_availableExtensions[i].extensionName));
        }
        
        return true;
    }

    bool VulkanPhysicalLayer::GetBestDevice(std::vector<VkPhysicalDevice>& p_devices)
    {
        for (int i = 0; i < p_devices.size(); i++)
        {
            if (DeviceMatched(p_devices[i]))
            {
                _physicalDevice = p_devices[i];
                return true;
            }
        }

        return false;
    }
    
    bool VulkanPhysicalLayer::DeviceMatched(const VkPhysicalDevice& p_device) const
    {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(p_device, &deviceProperties);
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(p_device, &deviceFeatures);

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(p_device, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> deviceExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(p_device, nullptr, &extensionCount, deviceExtensions.data());
        
        for (uint32_t i = 0; i < _enabledExtensionNames.size(); i++)
        {
            bool isAvailable = false;
            for (uint32_t j = 0; j < deviceExtensions.size(); j++)
            {
                if (strcmp(_enabledExtensionNames[i], deviceExtensions[j].extensionName) == 0)
                {
                    isAvailable = true;
                    break;
                }
            }

            if (!isAvailable)
            {
                return false;
            }
        }

        return true;
    }

    bool VulkanPhysicalLayer::IsExtensionAvailable(const char* p_extensionName) const
    {
        for (int i = 0; i < _availableExtensions.size(); i++)
        {
            if (strcmp(_availableExtensions[i].extensionName, p_extensionName) == 0)
            {
                return true;
            }
        }
            
        LOG_WARN(_logger, "Extension \"{0}\" is unavailable!", p_extensionName);
        return false;
    }
    
    bool VulkanPhysicalLayer::TryEnableExtension(const char* p_extensionName)
    {
        if (IsExtensionsEnabled(p_extensionName))
        {
            LOG_TRACE(_logger, "Trying to enable already enabled \"{0}\" extension!", p_extensionName);
            return true;
        }
        
        if (IsExtensionAvailable(p_extensionName))
        {
            LOG_TRACE(_logger, "Add \"{0}\" extension to enable", p_extensionName);
            _enabledExtensionNames.push_back(p_extensionName);
            return true;
        }

        LOG_ERR(_logger, "Failed to enable \"{0}\" extension!", p_extensionName);
        return false;
    }

    bool VulkanPhysicalLayer::IsExtensionsEnabled(const char* p_extensionName) const
    {
        for (int i = 0; i < _enabledExtensionNames.size(); i++)
        {
            if (strcmp(_enabledExtensionNames[i], p_extensionName) == 0)
            {
                return true;
            }
        }
        return false;
    }
    
    inline bool VulkanPhysicalLayer::IsExtensionAvailable(const VkExtensionProperties& p_extension) const
    { return IsExtensionAvailable(p_extension.extensionName); }
        
    inline bool VulkanPhysicalLayer::IsExtensionAvailable(const std::string& p_extensionName) const 
    { return IsExtensionAvailable(p_extensionName.c_str()); }
    
    inline bool VulkanPhysicalLayer::TryEnableExtension(const VkExtensionProperties& p_extension)
    { return TryEnableExtension(p_extension.extensionName); }
    
    inline bool VulkanPhysicalLayer::TryEnableExtension(const std::string& p_extensionName)
    { return TryEnableExtension(p_extensionName.c_str()); }
}
