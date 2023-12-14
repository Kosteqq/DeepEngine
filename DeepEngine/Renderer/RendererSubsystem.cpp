#include "RendererSubsystem.h"

namespace DeepEngine::Renderer
{
    bool RendererSubsystem::InitializeVulkanInstance()
    {
        MESSENGER_PREINITIALIZE(VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);
            
        Vulkan::VulkanDebugger::TryAddValidationLayer("VK_LAYER_KHRONOS_validation");

        if (_vulkanInstance->IsInstanceExtensionAvailable(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        {
            _vulkanInstance->EnableInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        if (!EnableGlfwExtensions())
        {
            return false;
        }
            
        if (!_vulkanInstance->InitializeInstance())
        {
            return false;
        }

        _vulkanInstance->EnablePhysicalExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        if (!_vulkanInstance->InitializePhysicalDevice())
        {
            return false;
        }

        if (!_vulkanInstance->TryAddQueueToCreate(VK_QUEUE_GRAPHICS_BIT, true, &_mainGraphicsQueue))
        {
            return false;
        }

        if (!_vulkanInstance->InitializeLogicalDevice())
        {
            return false;
        }

        const auto& availableFormats = _vulkanInstance->GetAvailableSurfaceFormats();
        VkSurfaceFormatKHR bestFormat = availableFormats[0];
        for (int i = 0; i < (uint32_t)availableFormats.size(); i++)
        {
            if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB
                && availableFormats[i].colorSpace == VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT)
            {
                bestFormat = availableFormats[i];
                break;
            }
        }

        _vulkanInstance->SetSwapChainFormat(bestFormat, VK_PRESENT_MODE_MAILBOX_KHR, true);

        if (!_vulkanInstance->InitializeSwapChain())
        {
            return false;
        }

        return true;
    }

    bool RendererSubsystem::EnableGlfwExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        for (int i = 0; i < glfwExtensionCount; i++)
        {
            if (!_vulkanInstance->IsInstanceExtensionAvailable(glfwExtensions[i]))
            {
                return false;
            }

            _vulkanInstance->EnableInstanceExtension(glfwExtensions[i]);
        }

        return true;
    }
}
