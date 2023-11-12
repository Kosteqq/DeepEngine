#include "VulkanInstance.h"

namespace DeepEngine::Renderer::Vulkan
{
    bool VulkanInstance::InitializeInstance()
    {
        VULKAN_INFO("Initializing Vulkan instance");
        
        for (int i = 0; i < (uint32_t)_availableInstanceExtensions.size(); i++)
        {
            VULKAN_TRACE("{:<45} (v.{}): [{}]",
                _availableInstanceExtensions[i].extensionName,
                _availableInstanceExtensions[i].specVersion,
                IsNameInCollection(
                    _availableInstanceExtensions[i].extensionName,
                    _availableInstanceExtensions,
                    [](VkExtensionProperties& prop) { return prop.extensionName; }));
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
        instanceCreateInfo.enabledExtensionCount = (uint32_t)_enabledInstanceExtensionNames.size();
        instanceCreateInfo.ppEnabledExtensionNames = _enabledInstanceExtensionNames.data();
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
    
    bool VulkanInstance::IsInstanceExtensionAvailable(const char* p_extensionName) const
    {
        return IsNameInCollection(p_extensionName, _availableInstanceExtensions, [](const char* name) { return name; });
    }

    inline void VulkanInstance::EnableInstanceExtension(const VkExtensionProperties& p_extension)
    {
        EnableInstanceExtension(p_extension.extensionName);
    }
    
    void VulkanInstance::EnableInstanceExtension(const char* p_extensionName)
    {
        VULKAN_TRACE("Add \"{}\" extension to enabled", p_extensionName);
        _enabledInstanceExtensionNames.push_back(p_extensionName);
    }
}
