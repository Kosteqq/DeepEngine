#pragma once
#include "Architecture/EngineSystem.h"
#include "Vulkan_Instance/VulkanInstance.h"

#define MESSENGER_UTILS
#include "Vulkan_Debug/VulkanDebug.h"

namespace DeepEngine::Renderer
{
    class RendererSubsystem : Architecture::EngineSubsystem
    {
    public:
        RendererSubsystem() : EngineSubsystem("Renderer")
        {
        }

    protected:
        bool Init() override
        {
            MESSENGER_PREINITIALIZE(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);
            
            Vulkan::VulkanDebugger::TryAddValidationLayer("VK_LAYER_KHRONOS_validation");

            if (_vulkanInstance.IsInstanceExtensionAvailable(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
            {
                _vulkanInstance.EnableInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }

            if (!EnableGlfwExtensions())
            {
                return false;
            }
            
            if (!_vulkanInstance.InitializeInstance())
            {
                return false;
            }

            _vulkanInstance.EnablePhysicalExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

            if (!_vulkanInstance.InitializePhysicalDevice())
            {
                return false;
            }

            if (!_vulkanInstance.TryAddQueueToCreate(VK_QUEUE_GRAPHICS_BIT, true,
                &_graphicsQueue))
            {
                return false;
            }

            if (!_vulkanInstance.InitializeLogicalDevice())
            {
                return false;
            }
            
            return true;
        }
        
        void Destroy() override
        {
            Vulkan::VulkanDebugger::Terminate();
        }
        
        void Tick() override
        { }

    private:
        bool EnableGlfwExtensions()
        {
            uint32_t glfwExtensionCount = 0;
            const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

            for (int i = 0; i < glfwExtensionCount; i++)
            {
                if (!_vulkanInstance.IsInstanceExtensionAvailable(glfwExtensions[i]))
                {
                    return false;
                }

                _vulkanInstance.EnableInstanceExtension(glfwExtensions[i]);
            }

            return true;
        }

    private:
        Vulkan::VulkanInstance _vulkanInstance;
        const Vulkan::VulkanInstance::QueueInstance* _graphicsQueue;
    };
}
