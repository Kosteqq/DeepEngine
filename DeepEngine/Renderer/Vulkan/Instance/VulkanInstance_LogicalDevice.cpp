#include <stack>

#include "VulkanInstance.h"

namespace DeepEngine::Renderer::Vulkan
{
    void VulkanInstance::PreinitializeLogicalDevice()
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, nullptr);

        _availableQueueFamilies = std::vector<VkQueueFamilyProperties>(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queueFamilyCount, _availableQueueFamilies.data());
    }
    
    bool VulkanInstance::OnInitializeLogicalDevice()
    {
        std::stack<void*> nextPtrStack;
        nextPtrStack.push(nullptr);

        VkPhysicalDeviceDepthClipEnableFeaturesEXT enableDepthClipFeature { };
        if (IsPhysicalExtensionEnabled(VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME))
        {
            enableDepthClipFeature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT;
            enableDepthClipFeature.pNext = nextPtrStack.top();
            enableDepthClipFeature.depthClipEnable = VK_TRUE;
            nextPtrStack.push(&enableDepthClipFeature);
        }

        std::vector<const char*> extensions(_enabledPhysicalExtensionNames.size());
        for (uint32_t i = 0; i < _enabledPhysicalExtensionNames.size(); i++)
        {
            extensions[i] = _enabledPhysicalExtensionNames[i].c_str();
        }
        
        VkDeviceCreateInfo createInfo { };
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pNext = nextPtrStack.top();
        createInfo.pQueueCreateInfos = _queuesCreateInfo.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(_queuesCreateInfo.size());
        createInfo.pEnabledFeatures = &_physicalDeviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        createInfo.enabledLayerCount = MESSENGER_VALIDATION_LAYERS_SIZE;
        createInfo.ppEnabledLayerNames = MESSENGER_VALIDATION_LAYERS_DATA;

        VULKAN_CHECK_CREATE(
            vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_logicalDevice),
            "Failed to create logical device!")

        for (uint32_t i = 0; i < _queuesCreateInfo.size(); i++)
        {
            VkQueue queue;
            vkGetDeviceQueue(_logicalDevice, _queuesCreateInfo[i].queueFamilyIndex, 0, &queue);
            _queueInstances[i].Queue = queue;
        }
        return true;
    }

    void VulkanInstance::TerminateLogicalDevice()
    {
        vkDestroyDevice(_logicalDevice, nullptr);
    }

    bool VulkanInstance::TryAddQueueToCreate(const VkQueueFlagBits p_requiredFeatures, const bool p_needSurfaceSupport,
                                             const QueueInstance ** p_outputInstance)
    {
        for (uint32_t i = 0; i < _availableQueueFamilies.size(); i++)
        {
            VkBool32 supportSurfaces;
            vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, i, _surface, &supportSurfaces);

            if (p_needSurfaceSupport && !supportSurfaces)
            {
                continue;
            }

            if ((_availableQueueFamilies[i].queueFlags & p_requiredFeatures) == p_requiredFeatures)
            {
                VkDeviceQueueCreateInfo createInfo { };
                createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                createInfo.queueFamilyIndex = i;
                createInfo.queueCount = 1;
                createInfo.pQueuePriorities = &_queuePriority;

                uint32_t id = (uint32_t)_queuesCreateInfo.size();
                _queuesCreateInfo.push_back(createInfo);

                uint32_t queuePropertiesCount;
                vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queuePropertiesCount, nullptr);
                std::vector<VkQueueFamilyProperties> properties(queuePropertiesCount);
                vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &queuePropertiesCount, properties.data());
                
                _queueInstances.push_back(QueueInstance{
                    VK_NULL_HANDLE,
                    p_requiredFeatures,
                    properties,
                    static_cast<bool>(supportSurfaces),
                    id,
                    i
                });
                
                *p_outputInstance = &_queueInstances.back();
                return true;
            }
        }
        
        p_outputInstance = nullptr;
        return false;
    }
}
