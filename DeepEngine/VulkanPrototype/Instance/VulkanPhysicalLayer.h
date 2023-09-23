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

        inline bool IsExtensionAvailable(const VkExtensionProperties& p_extension) const;
        inline bool IsExtensionAvailable(const std::string& p_extensionName) const; 
        bool IsExtensionAvailable(const char* p_extensionName) const;

        inline bool TryEnableExtension(const VkExtensionProperties& p_extension);
        inline bool TryEnableExtension(const std::string& p_extensionName);
        bool TryEnableExtension(const char* p_extensionName);
        
        bool IsExtensionsEnabled(const char* p_extensionName) const;
        
        const std::vector<const char*>& GetEnabledExtensionNames() const
        { return _enabledExtensionNames; }

    private:
        bool GetBestDevice(std::vector<VkPhysicalDevice>& p_devices);
        bool DeviceMatched(const VkPhysicalDevice& p_device) const;

    private:
        VulkanInstance* _instance = nullptr;
        VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;

        std::vector<VkExtensionProperties> _availableExtensions;
        std::vector<const char*> _enabledExtensionNames;
        
        std::shared_ptr<Core::Debug::Logger> _logger;
    };
}
