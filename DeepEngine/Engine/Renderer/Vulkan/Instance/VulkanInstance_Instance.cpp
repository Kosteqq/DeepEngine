#include "VulkanInstance.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{
    void VulkanInstance::PreinitializeInstance()
    {
        uint32_t extensionsCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);

        _availableInstanceExtensions = std::vector<VkExtensionProperties>(extensionsCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, _availableInstanceExtensions.data());
        
        VULKAN_TRACE("Found {0} instance extensions:", extensionsCount);
        for (int i = 0; i < extensionsCount; i++)
        {
            VULKAN_TRACE("\t{:<45} (v.{})", _availableInstanceExtensions[i].extensionName, _availableInstanceExtensions[i].specVersion);
        }
    }

    bool VulkanInstance::OnInitializeInstance()
    {
        VULKAN_INFO("Initializing Vulkan instance");
        
        VULKAN_TRACE("Enabled instance extensions:");
        for (int i = 0; i < (uint32_t)_enabledInstanceExtensionNames.size(); i++)
        {
            VULKAN_TRACE("\t{:<45}", _enabledInstanceExtensionNames[i]);
        }

        std::vector<const char*> extensions(_enabledInstanceExtensionNames.size());
        for (uint32_t i = 0; i < _enabledInstanceExtensionNames.size(); i++)
        {
            extensions[i] = _enabledInstanceExtensionNames[i].c_str();
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
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
        instanceCreateInfo.enabledLayerCount = MESSENGER_VALIDATION_LAYERS_SIZE;
        instanceCreateInfo.ppEnabledLayerNames = MESSENGER_VALIDATION_LAYERS_DATA;
        instanceCreateInfo.pNext = MESSENGER_CREATE_INFO;

        VULKAN_CHECK_CREATE(
            vkCreateInstance(&instanceCreateInfo, nullptr, &_instance),
            "Failed to create Vulkan Instance")
        
        return true;
    }

    void VulkanInstance::TerminateInstance()
    {
        VULKAN_INFO("Terminating Vulkan instance");
        vkDestroyInstance(_instance, nullptr);
    }

    inline bool VulkanInstance::IsInstanceExtensionAvailable(const VkExtensionProperties& p_extension) const
    {
        return IsInstanceExtensionAvailable(p_extension.extensionName);
    }
    
    bool VulkanInstance::IsInstanceExtensionAvailable(const std::string& p_extensionName) const
    {
        for (uint32_t i = 0; i < _availableInstanceExtensions.size(); i++)
        {
            if (std::string(_availableInstanceExtensions[i].extensionName) == p_extensionName)
            {
                return true;
            }
        }
        return false;
    }

    inline void VulkanInstance::EnableInstanceExtension(const VkExtensionProperties& p_extension)
    {
        EnableInstanceExtension(p_extension.extensionName);
    }
    
    void VulkanInstance::EnableInstanceExtension(const std::string& p_extensionName)
    {
        VULKAN_TRACE("Add \"{}\" extension to enabled", p_extensionName);
        _enabledInstanceExtensionNames.push_back(p_extensionName);
    }
}
