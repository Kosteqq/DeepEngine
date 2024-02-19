#pragma once
#include "CommandPool.h"
#include "Controller/BaseVulkanController.h"

#define _VULKAN_COMMAND_BUFFER_CLASS_DEFINED

namespace DeepEngine::Engine::Renderer::Vulkan
{

    class CommandBuffer final : public BaseVulkanController
    {
    public:
        CommandBuffer(CommandPool* p_commandPool, bool p_createAsSecondary);
        ~CommandBuffer() override = default;

        const VkCommandBuffer& GetVkCommandBuffer() const
        { return _commandBuffer; }

    protected:
        bool OnInitialize() override;
        void OnTerminate() override
        {
            return;
        }

    private:
        VkCommandBuffer _commandBuffer = VK_NULL_HANDLE;

        const CommandPool* _commandPool;
        const bool _isSecondary;
    };

    class CommandBuffer2 : public VulkanObject
    {
    public:
        CommandBuffer2(const VkCommandBuffer p_handler)
            : _handler(p_handler)
        {
            // ...
        }

        VkCommandBuffer GetHandler() const
        {
            return _handler;
        }
        
    private:
        const VkCommandBuffer _handler;
    };

    template <>
    class VulkanFactory::SubFactory<CommandBuffer2>
    {
    public:
        static VulkanRef<CommandBuffer2> Create(VulkanRef<CommandPool2> p_commandPool, bool p_asSecondary = false)
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

        static std::vector<VulkanRef<CommandBuffer2>> CreateMany(VulkanRef<CommandPool2> p_commandPool, uint32_t p_amount, bool p_asSecondary = false)
        {
            VkCommandBufferAllocateInfo allocInfo { };
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = p_commandPool->GetHandler();
            allocInfo.level = p_asSecondary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
            allocInfo.commandBufferCount = 1;

            std::vector<VulkanRef<CommandBuffer2>> objects(p_amount);

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
        
    private:
        static void Terminate(VulkanObject* p_object)
        {
            // ...
        }
    };
    
}
