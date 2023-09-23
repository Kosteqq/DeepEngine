#include "VulkanLogicalLayer.h"
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

namespace DeepEngine::Renderer
{
    VulkanLogicalLayer::VulkanLogicalLayer(std::shared_ptr<Core::Debug::Logger> p_logger, const VulkanPhysicalLayer* p_physicalLayer)
        : _logger(p_logger), _physicalLayer(p_physicalLayer)
    {
        _createQueuesInfo.reserve(5);

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

        VkPhysicalDeviceFeatures deviceFeatures { };
        // duplicate required features

        VkDeviceCreateInfo createInfo { };
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = _createQueuesInfo.data();
        createInfo.queueCreateInfoCount = _createQueuesInfo.size();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledLayerCount = static_cast<uint32_t>(p_vulkanDebug->GetEnabledLayers().size());
        createInfo.ppEnabledLayerNames = p_vulkanDebug->GetEnabledLayers().data();

        const auto result = vkCreateDevice(_physicalLayer->GetDevice(), &createInfo, nullptr, &_logicalDevice);
        if (result != VK_SUCCESS)
        {
            LOG_ERR(_logger, "Failed to create Vulkan logical device.\nDriver returned result: {0}", string_VkResult(result));
            return false;
        }

        vkGetDeviceQueue(_logicalDevice, _createQueuesInfo[0].queueFamilyIndex, 0, &_graphicsQueue);
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

    void VulkanLogicalLayer::AddQueue(VkQueueFlagBits p_queueFeatures)
    {
        uint32_t familyIndex = 0;
        bool found = false;
        for (uint32_t i = 0; i < _availableQueueFamilies.size(); i++)
        {
            if (_availableQueueFamilies[i].queueFlags & p_queueFeatures)
            {
                familyIndex = i;
                found = true;
                break;
            }
        }

        if (!found)
        {
            LOG_ERR(_logger, "Failed to find proper queue family");
            return;
        }
        
        VkDeviceQueueCreateInfo queueCreateInfo { };
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = familyIndex;
        queueCreateInfo.queueCount = 1;

        // TODO (Kostek): Check when its useful
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        _createQueuesInfo.push_back(queueCreateInfo);
    }
}
