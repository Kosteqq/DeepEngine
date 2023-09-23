#include "VulkanInstance.h"
#include <vulkan/vk_enum_string_helper.h>

#include "Debugs/Logger.h"

namespace DeepEngine::Renderer
{
    void GetExtensions(std::vector<VkExtensionProperties>* p_extensions)
    {
    }
    
    VulkanInstance::VulkanInstance(std::shared_ptr<Core::Debug::Logger> p_logger)
        : _logger(p_logger)
    {
        uint32_t extensionsCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);

        _enabledExtensionNames.reserve(extensionsCount);
        _availableExtensions = std::vector<VkExtensionProperties>(extensionsCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, _availableExtensions.data());
    }

    VulkanInstance::~VulkanInstance()
    {
        Terminate();
    }

    bool VulkanInstance::Init(VulkanDebug* p_vulkanDebug)
    {
        LOG_DEBUG(_logger, "Creating Vulkan Instance");
        for (int i = 0; i < _availableExtensions.size(); i++)
        {
            LOG_DEBUG(_logger, "{:<45} (v.{}): [{}]",
                _availableExtensions[i].extensionName,
                _availableExtensions[i].specVersion,
                IsExtensionsEnabled(_availableExtensions[i].extensionName));
        }
        
        VkApplicationInfo appInfo;
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext = nullptr;
        appInfo.pApplicationName = "Deep Engine Editor";
        appInfo.applicationVersion = 0;
        appInfo.pEngineName = "Deep Engine";
        appInfo.engineVersion = 0;
        appInfo.apiVersion = VK_API_VERSION_1_3;
            
        VkInstanceCreateInfo instanceCreateInfo { };
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &appInfo;
        instanceCreateInfo.enabledExtensionCount = (uint32_t)_enabledExtensionNames.size();
        instanceCreateInfo.ppEnabledExtensionNames = _enabledExtensionNames.data();
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(p_vulkanDebug->GetEnabledLayers().size());
        instanceCreateInfo.ppEnabledLayerNames = p_vulkanDebug->GetEnabledLayers().data();
        instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&p_vulkanDebug->GetCreateInfo();

        const auto result = vkCreateInstance(&instanceCreateInfo, nullptr, &_vkInstance);
        if (result != VK_SUCCESS)
        {
            LOG_ERR(_logger, "Failed to create Vulkan instance.\nDriver returned result: {0}", string_VkResult(result));
            return false;
        }
        
        _initialized = true;
        return true;
    }

    void VulkanInstance::Terminate()
    {
        if (_initialized)
        {
            LOG_INFO(_logger, "Terminating Vulkan Instance");
            vkDestroyInstance(_vkInstance, nullptr);
            _initialized = false;
        }
    }

    bool VulkanInstance::IsExtensionAvailable(const char* p_extensionName) const
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
    
    bool VulkanInstance::TryEnableExtension(const char* p_extensionName)
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

    bool VulkanInstance::IsExtensionsEnabled(const char* p_extensionName) const
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
    
    inline bool VulkanInstance::IsExtensionAvailable(const VkExtensionProperties& p_extension) const
    { return IsExtensionAvailable(p_extension.extensionName); }
        
    inline bool VulkanInstance::IsExtensionAvailable(const std::string& p_extensionName) const 
    { return IsExtensionAvailable(p_extensionName.c_str()); }
    
    inline bool VulkanInstance::TryEnableExtension(const VkExtensionProperties& p_extension)
    { return TryEnableExtension(p_extension.extensionName); }
    
    inline bool VulkanInstance::TryEnableExtension(const std::string& p_extensionName)
    { return TryEnableExtension(p_extensionName.c_str()); }
}
