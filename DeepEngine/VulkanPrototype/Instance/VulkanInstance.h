#pragma once
#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "VulkanDebug.h"
#include "Debugs/Logger.h"

namespace DeepEngine::Renderer
{
    class VulkanInstance
    {
    public:
        VulkanInstance(std::shared_ptr<Core::Debug::Logger> p_logger);
        ~VulkanInstance();
        
        bool Init(VulkanDebug* p_vulkanDebug);
        void Terminate();

        inline bool IsExtensionAvailable(const VkExtensionProperties& p_extension) const;
        inline bool IsExtensionAvailable(const std::string& p_extensionName) const; 
        bool IsExtensionAvailable(const char* p_extensionName) const;

        inline bool TryEnableExtension(const VkExtensionProperties& p_extension);
        inline bool TryEnableExtension(const std::string& p_extensionName);
        bool TryEnableExtension(const char* p_extensionName);

        bool IsLayerEnabled(const char* p_layerName) const;


        const std::vector<VkExtensionProperties>& GetAvailableExtensions() const
        { return _availableExtensions; }

        const std::vector<const char*>& GetEnabledExtensionNames() const
        { return _enabledExtensionNames; }
        
        VkInstance& GetInstance()
        { return _vkInstance; }

    private:
        bool _initialized = false;
        VkInstance _vkInstance = VK_NULL_HANDLE;

        std::vector<VkExtensionProperties> _availableExtensions { };
        std::vector<const char*> _enabledExtensionNames { };
        
        std::shared_ptr<Core::Debug::Logger> _logger;
  };
}
