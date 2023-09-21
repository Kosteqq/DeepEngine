#pragma once

#include <vulkan/vulkan_core.h>
#include "Window/WindowSubsystem.hpp"

#include "../Architecture/EngineSystem.h"

namespace DeepEngine
{
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices
    {
        uint32_t graphicsFamily;
        uint32_t presentFamily;
        bool graphicsFamilyHasValue = false;
        bool presentFamilyHasValue = false;
        bool isComplete() {return graphicsFamilyHasValue && presentFamilyHasValue; }
    };
    
    class Device 
    {
    public:
#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
        
#endif
        Device();
        ~Device();
        
    protected:
      

    private:
        void initVulkan();
        bool CreateInstance();

        VkInstance _instance;
        uint32_t glfwExtensionsCount = 0;
        const char** glfwExtensions;
    };
}
