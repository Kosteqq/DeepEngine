#include "CommandPool.h"

namespace DeepEngine::Renderer::Vulkan
{
    CommandPool::CommandPool(const VulkanInstance::QueueInstance* p_queue, const CommandPoolFlag p_flags)
        : _queue(p_queue), _flag(p_flags)
    { }

    bool CommandPool::OnInitialize()
    {
        VkCommandPoolCreateInfo createInfo { };
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.flags = 0;
        createInfo.queueFamilyIndex = _queue->FamilyIndex;

        if (_flag & CommandPoolFlag::PROTECTED)
        {
            createInfo.flags |= VK_COMMAND_POOL_CREATE_PROTECTED_BIT;
        }
        if (_flag & CommandPoolFlag::TRANSIENT)
        {
            createInfo.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        }
        if (_flag & CommandPoolFlag::RESET_COMMAND_BUFFER)
        {
            createInfo.flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        }

        VULKAN_CHECK_CREATE(
            vkCreateCommandPool(
                GetVulkanInstanceController()->GetLogicalDevice(),
                &createInfo,
                nullptr,
                &_commandPool),
            "Failed to create Vulkan Command Pool")

        return true;
    }

    void CommandPool::OnTerminate()
    {
        vkDestroyCommandPool(GetVulkanInstanceController()->GetLogicalDevice(), _commandPool, nullptr);
    }
}
