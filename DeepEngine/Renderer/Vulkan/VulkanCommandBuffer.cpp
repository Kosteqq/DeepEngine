#include "VulkanCommandBuffer.h"

namespace DeepEngine::Renderer::Vulkan
{
    VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandPool* p_commandPool, bool p_createAsSecondary)
        : _commandPool(p_commandPool), _isSecondary(p_createAsSecondary)
    { }

    bool VulkanCommandBuffer::OnInitialize()
    {
        VkCommandBufferAllocateInfo allocInfo { };
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = _commandPool->GetVkCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (_isSecondary)
        {
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        }

        VULKAN_CHECK_CREATE(
            vkAllocateCommandBuffers(
                GetVulkanInstanceController()->GetLogicalDevice(),
                &allocInfo,
                &_commandBuffer),
            "Failed to Allocate Command Buffer")

        return true;
    }
}
