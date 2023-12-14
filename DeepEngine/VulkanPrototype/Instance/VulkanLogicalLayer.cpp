#include "VulkanLogicalLayer.h"
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

namespace DeepEngine::Renderer
{
    VulkanLogicalLayer::VulkanLogicalLayer(std::shared_ptr<Debug::Logger> p_logger, const VulkanPhysicalLayer* p_physicalLayer,
            const VkSurfaceKHR& p_surface)
        : _logger(p_logger), _physicalLayer(p_physicalLayer), _surface(p_surface)
    {
        _createQueuesInfo.reserve(5);
        _createQueueIds.reserve(5);
        _queueMap.reserve(5);

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(p_physicalLayer->GetDevice(), &queueFamilyCount, nullptr);

        _availableQueueFamilies = std::vector<VkQueueFamilyProperties>(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(p_physicalLayer->GetDevice(), &queueFamilyCount, _availableQueueFamilies.data());
    }

    VulkanLogicalLayer::~VulkanLogicalLayer()
    {
        Terminate();
    }

    bool VulkanLogicalLayer::Init(const VulkanDebug* p_vulkanDebug)
    {
        LOG_DEBUG(_logger, "Initializing logical device");

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(_physicalLayer->GetDevice(), &deviceFeatures);

        VkDeviceCreateInfo createInfo { };
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = _createQueuesInfo.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(_createQueuesInfo.size());
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(_physicalLayer->GetEnabledExtensionNames().size());
        createInfo.ppEnabledExtensionNames = _physicalLayer->GetEnabledExtensionNames().data();
        createInfo.enabledLayerCount = static_cast<uint32_t>(p_vulkanDebug->GetEnabledLayers().size());
        createInfo.ppEnabledLayerNames = p_vulkanDebug->GetEnabledLayers().data();

        const auto result = vkCreateDevice(_physicalLayer->GetDevice(), &createInfo, nullptr, &_logicalDevice);
        if (result != VK_SUCCESS)
        {
            LOG_ERR(_logger, "Failed to create Vulkan logical device.\nDriver returned result: {0}", string_VkResult(result));
            return false;
        }

        for (uint32_t i = 0; i < _createQueuesInfo.size(); i++)
        {
            VkQueue queue;
            vkGetDeviceQueue(_logicalDevice, _createQueuesInfo[i].queueFamilyIndex, 0, &queue);
            _queueMap[_createQueueIds[i]] = queue;
        }
        return true;
    }

    void VulkanLogicalLayer::Terminate()
    {
        if (!_initialized)
        {
            _initialized = false;
            LOG_DEBUG(_logger, "Terminating logical device");
            vkDestroyDevice(_logicalDevice, nullptr);
        }
    }

    bool VulkanLogicalLayer::AddQueue(uint32_t p_id, VkQueueFlagBits p_queueFeatures, bool p_requireSurfaceSupport)
    {
        uint32_t familyIndex = 0;
        bool found = false;
        for (uint32_t i = 0; i < _availableQueueFamilies.size(); i++)
        {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(_physicalLayer->GetDevice(), i, _surface, &presentSupport);
            
            if ((presentSupport || !p_requireSurfaceSupport) && _availableQueueFamilies[i].queueFlags & p_queueFeatures)
            {
                familyIndex = i;
                found = true;
                break;
            }
        }

        if (!found)
        {
            LOG_ERR(_logger, "Failed to find proper queue family");
            return false;
        }
        
        VkDeviceQueueCreateInfo queueCreateInfo { };
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = familyIndex;
        queueCreateInfo.queueCount = 1;

        // TODO (Kostek): Check when its useful
        queueCreateInfo.pQueuePriorities = &_queuePriority;

        _createQueuesInfo.push_back(queueCreateInfo);
        _createQueueIds.push_back(p_id);
        return true;
    }
}
