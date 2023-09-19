#include "Device.hpp"
#include "../Debug/Debug.h"

//std lib headers
#include <vector>

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
    
    Device::Device()
    {
        vkDestroyInstance(_instance, nullptr);
    }

    
    void Device::initVulkan()
    {
        CreateInstance();
    }

    void Device::CreateInstance()
    {
        VkApplicationInfo appInfo{};
        appInfo.pNext = nullptr;
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Deep Engine";
        appInfo.applicationVersion = VK_MAKE_VERSION(0,1,0);
        appInfo.pEngineName = "Deep Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(0,1,0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pNext = nullptr;
        // create_info.flags = ;
        create_info.pApplicationInfo = &appInfo;
        create_info.enabledLayerCount = 0;
        // create_info.enabledExtensionCount = ; 
        // create_info.ppEnabledLayerNames =  ;
        // create_info.enabledExtensionCount =  ;
        // create_info.ppEnabledExtensionNames =  ;

        VkResult result = vkCreateInstance(&create_info, nullptr, &_instance);

        if(vkCreateInstance(&create_info, nullptr, &_instance) != VK_SUCCESS)
        {
            LOG("failed to create instance!")
        }

        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
        
        LOG("avaible extensions: /n")

        for(const auto& extension : extensions)
        {
            LOG(extension.extensionName)
        }
    }

    
}
