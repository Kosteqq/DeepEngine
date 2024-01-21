#include "VulkanDebug.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{
    void VulkanDebugger::PreInitialize(int p_logLevels,
        int p_logTypes)
    {
        VulkanDebugger& instance = GetInstance();

        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        instance._availableValidationLayers = std::vector<VkLayerProperties>(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, instance._availableValidationLayers.data());
        
        VkDebugUtilsMessengerCreateInfoEXT createInfo { };
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = p_logLevels;
        createInfo.messageType = p_logTypes;
        createInfo.pfnUserCallback = VulkanDebugMessengerCallback;
        createInfo.pUserData = nullptr;
        
        instance._messengerCreateInfo = createInfo;
    }

    void VulkanDebugger::Initialize(VkInstance p_vkInstance)
    {
        VulkanDebugger& instance = GetInstance();
        instance._vulkanInstance = p_vkInstance;
        
        LOG_DEBUG(instance._logger, "Creating Vulkan messenger!");
        LOG_TRACE(instance._logger, "Available validation layers:");
        for (int i = 0; i < instance._availableValidationLayers.size(); i++)
        {
            VkLayerProperties layer = instance._availableValidationLayers[i];
            LOG_TRACE(instance._logger, "{:<45} (v.{}): [{}]",
                layer.layerName,
                layer.specVersion,
                instance.IsLayerEnabled(layer.layerName));
        }

        const auto result = CreateDebugUtilsMessengerEXT(instance._vulkanInstance, &instance._messengerCreateInfo, nullptr, &instance._callbackMessenger);
        if (result != VK_SUCCESS)
        {
            LOG_ERR(instance._logger, "Failed to create Vulkan debug messenger.\nDriver returned result: {0}", string_VkResult(result));
        }
    }

    void VulkanDebugger::Terminate()
    {
        VulkanDebugger& instance = GetInstance();
        LOG_INFO(instance._logger, "Terminating Vulkan debug messenger");
        DestroyDebugUtilsMessengerEXT(instance._vulkanInstance, instance._callbackMessenger, nullptr);
    }

    inline bool VulkanDebugger::TryAddValidationLayer(const VkLayerProperties& p_layer)
    { return TryAddValidationLayer(p_layer.layerName); }
    
    bool VulkanDebugger::TryAddValidationLayer(const char* p_layerName)
    {
        VulkanDebugger& instance = GetInstance();
        if (instance.IsLayerEnabled(p_layerName))
        {
            LOG_TRACE(instance._logger, "Trying to enable already enabled \"{0}\" layer!", p_layerName);
            return true;
        }
        
        for (int i = 0; i < instance._availableValidationLayers.size(); i++)
        {
            if (strcmp(instance._availableValidationLayers[i].layerName, p_layerName) == 0)
            {
                LOG_TRACE(instance._logger, "Add \"{0}\" layer to enable", p_layerName);
                instance._enabledValidationLayers.push_back(p_layerName);
                return true;
            }
        }

        LOG_ERR(instance._logger, "Failed to enable \"{0}\" layer!", p_layerName);
        return false;
    }
    
    bool VulkanDebugger::IsLayerEnabled(const char* p_layerName) const
    {
        for (int i = 0; i < _enabledValidationLayers.size(); i++)
        {
            if (strcmp(_enabledValidationLayers[i], p_layerName) == 0)
            {
                return true;
            }
        }
        return false;
    }

    VkBool32 VulkanDebugger::VulkanDebugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT p_messageLevel,
        VkDebugUtilsMessageSeverityFlagsEXT p_messageType, const VkDebugUtilsMessengerCallbackDataEXT* p_callbackData,
        void* p_usePtr)
    {
        const VulkanDebugger& instance = GetInstance();
        std::string messageType;
        switch (p_messageType)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            messageType = "Trace";
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            messageType = "Info";
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            messageType = "Warning";
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            messageType = "Error";
            break;
        }
        
        if (p_messageLevel == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        {
            LOG_TRACE(instance._messengerCallbackLogger, "({0}) {1}", messageType, p_callbackData->pMessage);
        }
        else if (p_messageLevel == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        {
            LOG_INFO(instance._messengerCallbackLogger, "({0}) {1}", messageType, p_callbackData->pMessage);
        }
        else if (p_messageLevel == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            LOG_WARN(instance._messengerCallbackLogger, "({0}) {1}", messageType, p_callbackData->pMessage);
        }
        else if (p_messageLevel == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            LOG_ERR(instance._messengerCallbackLogger, "({0}) {1}", messageType, p_callbackData->pMessage);
        }
            
        return VK_FALSE;
    }

    
    VkResult VulkanDebugger::CreateDebugUtilsMessengerEXT(VkInstance p_instance, const VkDebugUtilsMessengerCreateInfoEXT* p_createInfo,
        const VkAllocationCallbacks* p_allocator, VkDebugUtilsMessengerEXT* p_debugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(p_instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(p_instance, p_createInfo, p_allocator, p_debugMessenger);
        }
            
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
        
    void VulkanDebugger::DestroyDebugUtilsMessengerEXT(VkInstance p_instance, VkDebugUtilsMessengerEXT p_debugMessenger,
        const VkAllocationCallbacks* p_allocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(p_instance, "vkDestroyDebugUtilsMessengerEXT");
            
        if (func != nullptr)
        {
            func(p_instance, p_debugMessenger, p_allocator);
        }
    }
}
