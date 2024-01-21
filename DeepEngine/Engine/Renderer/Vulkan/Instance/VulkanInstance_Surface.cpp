#include "VulkanInstance.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{
    bool VulkanInstance::OnInitializeSurface()
    {
        glfwCreateWindowSurface(_instance, _glfwWindow, nullptr, &_surface);
        

        uint32_t formatCount;
        uint32_t presentModeCount;
        
        vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatCount, nullptr);
        vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &presentModeCount, nullptr);

        if (formatCount == 0 || presentModeCount == 0)
        {
            VULKAN_ERR("Failed to get any available surface formats ({}) of presents modes ({})!", formatCount, presentModeCount);
            return false;
        }

        _availableSurfaceFormats.resize(formatCount);
        _availableSurfacePresentModes.resize(presentModeCount);

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, _surface, &_availableSurfaceCapabilities);
        vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &formatCount, _availableSurfaceFormats.data());
        vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &presentModeCount, _availableSurfacePresentModes.data());
        return true;
    }
}
