#pragma once
#include "VulkanPCH.h"

#include <memory>
#include <fmt/format.h>

#include "Debugs/Logger.h"

#define VULkAN_TRACE(mess, ...) LOG_TRACE(DeepEngine::Renderer::Vulkan::VulkanDebugger::GetInstance(), mess, __VA_ARGS___)
#define VULkAN_DEBUG(mess, ...) LOG_DEBUG(DeepEngine::Renderer::Vulkan::VulkanDebugger::GetInstance(), mess, __VA_ARGS___)
#define VULkAN_INFO(mess, ...) LOG_INFO(DeepEngine::Renderer::Vulkan::VulkanDebugger::GetInstance(), mess, __VA_ARGS___)
#define VULkAN_WARN(mess, ...) LOG_WARN(DeepEngine::Renderer::Vulkan::VulkanDebugger::GetInstance(), mess, __VA_ARGS___)
#define VULkAN_ERR(mess, ...) LOG_ERR(DeepEngine::Renderer::Vulkan::VulkanDebugger::GetInstance(), mess, __VA_ARGS___)

#define VULKAN_CHECK_CREATE(result, message, ...)               \
    if (result != VK_SUCCESS)                                   \
    {                                                           \
        VULKAN_ERR("Vulkan create assertion failed!\n"          \
                "Returned result: {}\n"                         \
                "Message: {}",                                  \
                string_VkResult(result), message, __VA_ARGS__); \
        return false;                                           \
    }                                                           \

#ifdef MESSENGER_UTILS
#endif
#   define VULKAN_PREINITIALIZE_MESSENGER(LogLevels, LogTypes) DeepEngine::Renderer::Vulkan::VulkanDebugger::PreInitialize(LogLevels, LogTypes) 
#   define VULKAN_INITIALIZE_MESSENGER(VkInstance) DeepEngine::Renderer::Vulkan::VulkanDebugger::Initialize(VkInstance)
#   define ENABLED_MESSENGER_VALIDATION_LAYERS DeepEngine::Renderer::Vulkan::VulkanDebugger::GetEnabledValidationLayers()
#   define MESSENGER_CREATE_INFO DeepEngine::Renderer::Vulkan::VulkanDebugger::GetMessengerCreateInfo()
        

namespace DeepEngine::Renderer::Vulkan
{
    
    class VulkanDebugger
    {
    private:
        VulkanDebugger()
        {
            _logger = Debug::Logger::CreateLoggerInstance("Vulkan Debug");
        }
        
    public:
        static void PreInitialize(const VkDebugUtilsMessageTypeFlagBitsEXT p_logLevels,
            const VkDebugUtilsMessageTypeFlagsEXT p_logTypes);

        static void Initialize(VkInstance p_vkInstance);
        static void Terminate();

        inline bool TryAddValidationLayer(const VkLayerProperties& p_layer);
        inline bool TryAddValidationLayer(const std::string& p_layerName);
        bool TryAddValidationLayer(const char* p_layerName);
        
        static Debug::Logger* GetLogger()
        { return GetInstance()._logger.get(); }

        static const std::vector<const char*>& GetEnabledValidationLayers()
        { return GetInstance()._enabledValidationLayers; }

        static VkDebugUtilsMessengerCreateInfoEXT* GetMessengerCreateInfo()
        { return &GetInstance()._messengerCreateInfo; }

    private:
        bool IsLayerEnabled(const char* p_layerName) const;

        static VulkanDebugger& GetInstance()
        {
            static VulkanDebugger instance;
            return instance;
        }

        static VkResult CreateDebugUtilsMessengerEXT(VkInstance p_instance, const VkDebugUtilsMessengerCreateInfoEXT* p_createInfo,
            const VkAllocationCallbacks* p_allocator, VkDebugUtilsMessengerEXT* p_debugMessenger);
        static void DestroyDebugUtilsMessengerEXT(VkInstance p_instance, VkDebugUtilsMessengerEXT p_debugMessenger,
            const VkAllocationCallbacks* p_allocator);
        
        VkBool32 VulkanDebugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT p_messageLevel,
            VkDebugUtilsMessageSeverityFlagsEXT p_messageType, const VkDebugUtilsMessengerCallbackDataEXT* p_callbackData,
            void* p_usePtr);

    private:
        VkInstance _vulkanInstance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT _callbackMessenger = VK_NULL_HANDLE;
        VkDebugUtilsMessengerCreateInfoEXT _messengerCreateInfo { };

        std::vector<const char*> _enabledValidationLayers;
        std::vector<VkLayerProperties> _availableValidationLayers;
        
        std::shared_ptr<Debug::Logger> _logger;
        std::shared_ptr<Debug::Logger> _messengerCallbackLogger;
    };
    
}
