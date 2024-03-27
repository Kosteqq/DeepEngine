#include "CommandBuffer.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{

    Ref<CommandBuffer> Factory::SubFactory<CommandBuffer>::Create(Ref<CommandPool> p_commandPool,
        bool p_asSecondary)
    {
        VkCommandBufferAllocateInfo allocInfo { };
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = p_commandPool->GetHandler();
        allocInfo.level = p_asSecondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer bufferHandler;
        VULKAN_ASSERT_RESULT_V(
            vkAllocateCommandBuffers(
                _bindFactory->_vulkanInstance.GetLogicalDevice(),
                &allocInfo,
                &bufferHandler),
            nullptr)

        const auto bufferObject = new CommandBuffer(bufferHandler);
        return CreateObject(bufferObject, Terminate, p_commandPool);
    }

    std::vector<Ref<CommandBuffer>> Factory::SubFactory<CommandBuffer>::CreateMany(
        Ref<CommandPool> p_commandPool, uint32_t p_amount, bool p_asSecondary)
    {
        VkCommandBufferAllocateInfo allocInfo { };
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = p_commandPool->GetHandler();
        allocInfo.level = p_asSecondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        std::vector<Ref<CommandBuffer>> objects;
        objects.reserve(p_amount);

        for (uint32_t i = 0; i < p_amount; i++)
        {
            VkCommandBuffer bufferHandler;

            VULKAN_ASSERT_RESULT_CONTINUE(
                vkAllocateCommandBuffers(
                    _bindFactory->_vulkanInstance.GetLogicalDevice(),
                    &allocInfo,
                    &bufferHandler))

            const auto bufferObject = new CommandBuffer(bufferHandler);
            objects.push_back(CreateObject(bufferObject, Terminate, p_commandPool));
        }

        return objects;
    }

    void Factory::SubFactory<CommandBuffer>::Terminate(VulkanObject* p_object)
    {
        // ...
    }
}
