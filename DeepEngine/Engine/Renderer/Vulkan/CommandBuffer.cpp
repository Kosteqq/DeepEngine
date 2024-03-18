#include "CommandBuffer.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{
    CommandBuffer::CommandBuffer(CommandPool* p_commandPool, bool p_createAsSecondary)
        : _commandPool(p_commandPool), _isSecondary(p_createAsSecondary)
    { }

    bool CommandBuffer::OnInitialize()
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

    Ref<CommandBuffer2> VulkanFactory::SubFactory<CommandBuffer2>::Create(Ref<CommandPool2> p_commandPool,
        bool p_asSecondary)
    {
        VkCommandBufferAllocateInfo allocInfo { };
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = p_commandPool->GetHandler();
        allocInfo.level = p_asSecondary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer bufferHandler;
        vkAllocateCommandBuffers(
            _bindFactory->_vulkanInstance.GetLogicalDevice(),
            &allocInfo,
            &bufferHandler);

        auto bufferObject = new CommandBuffer2(bufferHandler);
        return CreateObject(bufferObject, Terminate, p_commandPool);
    }

    std::vector<Ref<CommandBuffer2>> VulkanFactory::SubFactory<CommandBuffer2>::CreateMany(
        Ref<CommandPool2> p_commandPool, uint32_t p_amount, bool p_asSecondary)
    {
        VkCommandBufferAllocateInfo allocInfo { };
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = p_commandPool->GetHandler();
        allocInfo.level = p_asSecondary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
        allocInfo.commandBufferCount = 1;

        std::vector<Ref<CommandBuffer2>> objects(p_amount);

        for (uint32_t i = 0; i < p_amount; i++)
        {
            VkCommandBuffer bufferHandler;
            vkAllocateCommandBuffers(
                _bindFactory->_vulkanInstance.GetLogicalDevice(),
                &allocInfo,
                &bufferHandler);
                
            auto bufferObject = new CommandBuffer2(bufferHandler);
                
            objects[i] = CreateObject(bufferObject, Terminate, p_commandPool);
        }

        return objects;
    }

    void VulkanFactory::SubFactory<CommandBuffer2>::Terminate(VulkanObject* p_object)
    {
        // ...
    }
}
