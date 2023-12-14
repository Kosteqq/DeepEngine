#include "VulkanDebug.h"
#include <fmt/format.h>
#include <vulkan/vk_enum_string_helper.h>

#define CALLBACK_FORMAT "({0}) {1}"


namespace DeepEngine::Renderer
{
    VkResult CreateDebugUtilsMessengerEXT(VkInstance p_instance, const VkDebugUtilsMessengerCreateInfoEXT* p_createInfo,
        const VkAllocationCallbacks* p_allocator, VkDebugUtilsMessengerEXT* p_debugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(p_instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(p_instance, p_createInfo, p_allocator, p_debugMessenger);
        }
            
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
        
    void DestroyDebugUtilsMessengerEXT(VkInstance p_instance, VkDebugUtilsMessengerEXT p_debugMessenger,
        const VkAllocationCallbacks* p_allocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(p_instance, "vkDestroyDebugUtilsMessengerEXT");
            
        if (func != nullptr)
        {
            func(p_instance, p_debugMessenger, p_allocator);
        }
    }
    
    bool IsLayerEnabled(const std::vector<const char*>& p_enabledLayers, const char* p_layerName)
    {
        for (int i = 0; i < p_enabledLayers.size(); i++)
        {
            if (strcmp(p_enabledLayers[i], p_layerName) == 0)
            {
                return true;
            }
        }
        return false;
    }
    
    VulkanDebug::VulkanDebug(std::shared_ptr<Debug::Logger> p_vulkanLogger,
            VkDebugUtilsMessageSeverityFlagsEXT p_logLevels, VkDebugUtilsMessageTypeFlagsEXT p_logTypes)
                : _vulkanLogger(p_vulkanLogger)
    {
        _debugVkLogger = Debug::Logger::CreateLoggerInstance("Vulkan Callback");

        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        _availableLayers = std::vector<VkLayerProperties>(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, _availableLayers.data());
        
        _createInfo = VkDebugUtilsMessengerCreateInfoEXT { };
        _createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        _createInfo.messageSeverity = p_logLevels;
        _createInfo.messageType = p_logTypes;
        _createInfo.pfnUserCallback = VulkanDebugCallback;
        _createInfo.pUserData = (void*)this;
    }

    VulkanDebug::~VulkanDebug()
    {
        Terminate();
    }

    bool VulkanDebug::Init(VkInstance* p_vkInstance)
    {
        _vkInstance = p_vkInstance;
        
        LOG_DEBUG(_vulkanLogger, "Creating Vulkan messenger");
        for (int i = 0; i < _availableLayers.size(); i++)
        {
            LOG_DEBUG(_vulkanLogger, "{:<45} (v.{}): [{}]",
                _availableLayers[i].layerName,
                _availableLayers[i].specVersion,
                IsLayerEnabled(_enabledLayers, _availableLayers[i].layerName));
        }

        const auto result = CreateDebugUtilsMessengerEXT(*_vkInstance, &_createInfo, nullptr, &_debugMessenger);
        if (result != VK_SUCCESS)
        {
            LOG_ERR(_vulkanLogger, "Failed to create Vulkan debug messenger.\nDriver returned result: {0}", string_VkResult(result));
            return false;
        }

        _initialized = true;
        return true;
    }

    void VulkanDebug::Terminate()
    {
        if (_initialized)
        {
            _initialized = false;
            LOG_INFO(_vulkanLogger, "Terminating Vulkan debug messenger");
            DestroyDebugUtilsMessengerEXT(*_vkInstance, _debugMessenger, nullptr);
        }
    }
    
    bool VulkanDebug::TryAddLayer(const char* p_layerName)
    {
        if (IsLayerEnabled(_enabledLayers, p_layerName))
        {
            LOG_TRACE(_vulkanLogger, "Trying to enable already enabled \"{0}\" layer!", p_layerName);
            return true;
        }
        
        if (IsLayerAvailable(p_layerName))
        {
            LOG_TRACE(_vulkanLogger, "Add \"{0}\" layer to enable", p_layerName);
            _enabledLayers.push_back(p_layerName);
            return true;
        }

        LOG_ERR(_vulkanLogger, "Failed to enable \"{0}\" layer!", p_layerName);
        return false;
    }

    bool VulkanDebug::IsLayerAvailable(const char* p_layerName) const
    {
        for (int i = 0; i < _availableLayers.size(); i++)
        {
            if (strcmp(_availableLayers[i].layerName, p_layerName) == 0)
            {
                return true;
            }
        }
            
        return false;
    }

    inline bool VulkanDebug::TryAddLayer(const VkLayerProperties& p_layer)
    { return TryAddLayer(p_layer.layerName); }

    inline bool VulkanDebug::TryAddLayer(const std::string& p_layerName)
    { return TryAddLayer(p_layerName); }

    inline bool VulkanDebug::IsLayerAvailable(const VkLayerProperties& p_layer) const
    { return IsLayerAvailable(p_layer.layerName); }

    inline bool VulkanDebug::IsLayerAvailable(const std::string& p_layerName) const
    { return IsLayerAvailable(p_layerName.c_str()); }

    VkBool32 VulkanDebug::VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT p_messageLevel,
        VkDebugUtilsMessageSeverityFlagsEXT p_messageType, const VkDebugUtilsMessengerCallbackDataEXT* p_callbackData,
        void* p_usePtr)
    {
        VulkanDebug* instance = (VulkanDebug*)p_usePtr;
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
            LOG_TRACE(instance->_debugVkLogger, CALLBACK_FORMAT, messageType, p_callbackData->pMessage);
        }
        else if (p_messageLevel == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        {
            LOG_INFO(instance->_debugVkLogger, CALLBACK_FORMAT, messageType, p_callbackData->pMessage);
        }
        else if (p_messageLevel == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            LOG_WARN(instance->_debugVkLogger, CALLBACK_FORMAT, messageType, p_callbackData->pMessage);
        }
        else if (p_messageLevel == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            LOG_ERR(instance->_debugVkLogger, CALLBACK_FORMAT, messageType, p_callbackData->pMessage);
        }
            
        return VK_FALSE;
    }
}
