#pragma once
#include "../VulkanPCH.h"

#include <memory>
#include <fmt/format.h>

#include "Debug/Logger.h"

#define VULKAN_ASSERT_OBJECT(p_object)                      ASSERT_COND_MSG(         (p_object)->IsValid(), "Failed Vulkan object assertion on {}", #p_object)
#define VULKAN_ASSERT_OBJECT_MSG(p_object, ...)             ASSERT_COND_MSG(         (p_object)->IsValid(), "Failed Vulkan object assertion on {}. Message {}", #p_object, __VA_ARGS__)
#define VULKAN_ASSERT_OBJECT_V(p_object, p_return)          ASSERT_COND_V_MSG(       (p_object)->IsValid(), p_return, "Failed Vulkan object assertion on {}", #p_object)
#define VULKAN_ASSERT_OBJECT_V_MSG(p_object, p_return, ...) ASSERT_COND_V_MSG(       (p_object)->IsValid(), p_return, "Failed Vulkan object assertion on {}. Message {}", #p_object, __VA_ARGS__)
#define VULKAN_ASSERT_OBJECT_BREAK(p_object)                ASSERT_COND_BREAK_MSG(   (p_object)->IsValid(), "Failed Vulkan object assertion on {}", #p_object)
#define VULKAN_ASSERT_OBJECT_BREAK_MSG(p_object, ...)       ASSERT_COND_BREAK_MSG(   (p_object)->IsValid(), "Failed Vulkan object assertion on {}. Message {}", #p_object, __VA_ARGS__)
#define VULKAN_ASSERT_OBJECT_CONTINUE(p_object)             ASSERT_COND_CONTINUE_MSG((p_object)->IsValid(), "Failed Vulkan object assertion on {}", #p_object)
#define VULKAN_ASSERT_OBJECT_CONTINUE_MSG(p_object, ...)    ASSERT_COND_CONTINUE_MSG((p_object)->IsValid(), "Failed Vulkan object assertion on {}. Message {}", #p_object, __VA_ARGS__)

#define VULKAN_ASSERT_RESULT(p_result)                      { VkResult result = p_result; ASSERT_COND_MSG(         (result) == VK_SUCCESS, "Vulkan result check returns {}", string_VkResult(result)) }
#define VULKAN_ASSERT_RESULT_MSG(p_result, ...)             { VkResult result = p_result; ASSERT_COND_MSG(         (result) == VK_SUCCESS, "Vulkan result check returns {}. Message: {}", string_VkResult(result), __VA_ARGS__) }
#define VULKAN_ASSERT_RESULT_V(p_result, p_return)          { VkResult result = p_result; ASSERT_COND_V_MSG(       (result) == VK_SUCCESS, p_return, "Vulkan result check returns {}", string_VkResult(result)) }
#define VULKAN_ASSERT_RESULT_V_MSG(p_result, p_return, ...) { VkResult result = p_result; ASSERT_COND_V_MSG(       (result) == VK_SUCCESS, p_return, "Vulkan result check returns {}. Message: {}", string_VkResult(result), __VA_ARGS__) }
#define VULKAN_ASSERT_RESULT_BREAK(p_result)                { VkResult result = p_result; ASSERT_COND_BREAK_MSG(   (result) == VK_SUCCESS, "Vulkan result check returns {}", string_VkResult(p_result)) }
#define VULKAN_ASSERT_RESULT_BREAK_MSG(p_result, ...)       { VkResult result = p_result; ASSERT_COND_BREAK_MSG(   (result) == VK_SUCCESS, "Vulkan result check returns {}. Message: {}", string_VkResult(result), __VA_ARGS__) }
#define VULKAN_ASSERT_RESULT_CONTINUE(p_result)             { VkResult result = p_result; ASSERT_COND_CONTINUE_MSG((result) == VK_SUCCESS, "Vulkan result check returns {}", string_VkResult(result)) }
#define VULKAN_ASSERT_RESULT_CONTINUE_MSG(p_result, ...)    { VkResult result = p_result; ASSERT_COND_CONTINUE_MSG((result) == VK_SUCCESS, "Vulkan result check returns {}. Message: {}", string_VkResult(result), __VA_ARGS__) }

#define VULKAN_TRACE(mess, ...) LOG_TRACE(DeepEngine::Engine::Renderer::Vulkan::VulkanDebugger::GetLogger(), mess, __VA_ARGS__)
#define VULKAN_DEBUG(mess, ...) LOG_DEBUG(DeepEngine::Engine::Renderer::Vulkan::VulkanDebugger::GetLogger(), mess, __VA_ARGS__)
#define VULKAN_INFO(mess, ...) LOG_INFO(DeepEngine::Engine::Renderer::Vulkan::VulkanDebugger::GetLogger(), mess, __VA_ARGS__)
#define VULKAN_WARN(mess, ...) LOG_WARN(DeepEngine::Engine::Renderer::Vulkan::VulkanDebugger::GetLogger(), mess, __VA_ARGS__)
#define VULKAN_ERR(mess, ...) LOG_ERR(DeepEngine::Engine::Renderer::Vulkan::VulkanDebugger::GetLogger(), mess, __VA_ARGS__)

#ifdef MESSENGER_UTILS
#endif
#   define MESSENGER_PREINITIALIZE(LogLevels, LogTypes) DeepEngine::Engine::Renderer::Vulkan::VulkanDebugger::PreInitialize(LogLevels, LogTypes) 
#   define MESSENGER_INITIALIZE(VkInstance) DeepEngine::Renderer::Engine::Vulkan::VulkanDebugger::Initialize(VkInstance)
#   define MESSENGER_VALIDATION_LAYERS_SIZE (uint32_t)DeepEngine::Engine::Renderer::Vulkan::VulkanDebugger::GetEnabledValidationLayers().size()
#   define MESSENGER_VALIDATION_LAYERS_DATA DeepEngine::Engine::Renderer::Vulkan::VulkanDebugger::GetEnabledValidationLayers().data()
#   define MESSENGER_CREATE_INFO DeepEngine::Engine::Renderer::Vulkan::VulkanDebugger::GetMessengerCreateInfo()
        

namespace DeepEngine::Engine::Renderer::Vulkan
{
    
    class VulkanDebugger
    {
    private:
        VulkanDebugger()
        {
            _logger = Debug::Logger::CreateLoggerInstance("VULKAN");
            _messengerCallbackLogger = Debug::Logger::CreateLoggerInstance("Vulkan Callback");
            
        }
        
    public:
        static void PreInitialize(int p_logLevels, int p_logTypes);

        static void Initialize(VkInstance p_vkInstance);
        static void Terminate();

        static inline bool TryAddValidationLayer(const VkLayerProperties& p_layer);
        static bool TryAddValidationLayer(const char* p_layerName);
        
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
        
        static VkBool32 VulkanDebugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT p_messageLevel,
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
