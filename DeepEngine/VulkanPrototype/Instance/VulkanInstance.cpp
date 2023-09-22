#include "VulkanInstance.h"
#include <vulkan/vk_enum_string_helper.h>

#include "Debugs/Logger.h"

namespace DeepEngine::Renderer
{
    void GetExtensions(std::vector<VkExtensionProperties>* p_extensions)
    {
        uint32_t extensionsCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);

        p_extensions->resize(extensionsCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, p_extensions->data());
    }

    bool IsExtensionEnabled(const std::vector<const char*>& p_enabledExtensions, const char* p_extensionName)
    {
        for (int i = 0; i < p_enabledExtensions.size(); i++)
        {
            if (strcmp(p_enabledExtensions[i], p_extensionName) == 0)
            {
                return true;
            }
        }
        return false;
    }
    
    VulkanInstance::VulkanInstance(std::shared_ptr<Core::Debug::Logger> p_logger)
        : _logger(p_logger)
    {
        _enabledExtensionNames.reserve(10);
        GetExtensions(&_availableExtensions);
    }

    VulkanInstance::~VulkanInstance()
    {
        Terminate();
    }

    bool VulkanInstance::Init(const std::vector<const char*>& p_validationLayers)
    {
        LOG_DEBUG(_logger, "Creating Vulkan Instance");
        for (int i = 0; i < _availableExtensions.size(); i++)
        {
            LOG_DEBUG(_logger, "{>20}{}: [{}]", _availableExtensions[i].extensionName, _availableExtensions[i].specVersion,
                IsExtensionEnabled(_enabledExtensionNames, _availableExtensions[i].extensionName));
        }
        
        VkApplicationInfo appInfo;
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pNext = nullptr;
        appInfo.pApplicationName = "Deep Engine Editor";
        appInfo.applicationVersion = 0;
        appInfo.pEngineName = "Deep Engine";
        appInfo.engineVersion = 0;
        appInfo.apiVersion = VK_API_VERSION_1_3;
            
        auto enabledExtensionNames = GetEnabledExtensionNames();

        VkInstanceCreateInfo instanceCreateInfo { };
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &appInfo;
        instanceCreateInfo.enabledExtensionCount = (uint32_t)_enabledExtensionNames.size();
        instanceCreateInfo.ppEnabledExtensionNames = _enabledExtensionNames.data();
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(p_validationLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = p_validationLayers.data();
        // instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&p_debugInfo;

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
            
        LOG_WARN(_logger, "Extension \"{0}\" is unabailable!", p_extensionName);
        return false;
    }
    
    bool VulkanInstance::TryEnableExtension(const char* p_extensionName)
    {
        if (IsExtensionEnabled(_enabledExtensionNames, p_extensionName))
        {
            LOG_TRACE(_logger, "Trying to enable already enabled \"{0}\" extension!", p_extensionName);
            return true;
        }
        
        if (IsExtensionAvailable(p_extensionName))
        {
            LOG_TRACE(_logger, "Add \"{0}\" extension to enable");
            _enabledExtensionNames.push_back(p_extensionName);
            return true;
        }

        LOG_ERR(_logger, "Failed to enable \"{0}\" extension!", p_extensionName);
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
