#pragma once
#include <vector>
#include <vulkan/vulkan.h>

#include "Debugs/Logger.h"

namespace DeepEngine::Renderer
{
    class VulkanDebug
    {
    public:
        VulkanDebug(std::shared_ptr<Core::Debug::Logger> p_vulkanLogger,
            VkDebugUtilsMessageSeverityFlagsEXT p_logLevels, VkDebugUtilsMessageTypeFlagsEXT p_logTypes);
        ~VulkanDebug();

        bool Init(VkInstance* p_vkInstance);
        void Terminate();
        
        inline bool TryAddLayer(const VkLayerProperties& p_layer);
        inline bool TryAddLayer(const std::string& p_layerName);
        bool TryAddLayer(const char* p_layerName);
        
        inline bool IsLayerAvailable(const VkLayerProperties& p_layer) const;
        inline bool IsLayerAvailable(const std::string& p_layerName) const;
        bool IsLayerAvailable(const char* p_layerName) const;

        const std::vector<VkLayerProperties>& GetAvailableLayers() const
        { return _availableLayers; }

        const std::vector<const char*>& GetEnabledLayers() const
        { return _enabledLayers; }

        const VkDebugUtilsMessengerCreateInfoEXT& GetCreateInfo() const
        { return _createInfo; }

    private:
        static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT p_messageLevel,
            VkDebugUtilsMessageSeverityFlagsEXT p_messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* p_callbackData,
            void* p_usePtr);

    private:
        bool _initialized = false;
        VkInstance* _vkInstance = nullptr;
        VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;
        VkDebugUtilsMessengerCreateInfoEXT _createInfo { };
        
        std::vector<const char*> _enabledLayers; 
        std::vector<VkLayerProperties> _availableLayers; 

        std::shared_ptr<Core::Debug::Logger> _debugVkLogger;
        std::shared_ptr<Core::Debug::Logger> _vulkanLogger;
    };
}
