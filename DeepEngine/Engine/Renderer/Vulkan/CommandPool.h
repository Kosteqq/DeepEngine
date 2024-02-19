#pragma once
#include "VulkanFactory.h"
#include "VulkanObject.h"
#include "Controller/BaseVulkanController.h"
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{

#ifndef _VULKAN_COMMAND_BUFFER_CLASS_DEFINED
    class CommandBuffer;
#endif

    enum CommandPoolFlag
    {
        TRANSIENT            = 1 << 0,
        RESET_COMMAND_BUFFER = 1 << 1,
        PROTECTED            = 1 << 2,
    };

    class CommandPool final : public BaseVulkanController
    {
    public:
        CommandPool(const VulkanInstance::QueueInstance* p_queue, const CommandPoolFlag p_flags);
        ~CommandPool() override = default;

        VkCommandPool GetVkCommandPool() const
        { return _commandPool; }

        std::vector<CommandBuffer*> CreateCommandBuffers(uint32_t p_buffersCount);

    protected:
        bool OnInitialize() override;
        void OnTerminate() override;

    private:
        VkCommandPool _commandPool;

        const VulkanInstance::QueueInstance* _queue;
        const CommandPoolFlag _flag;
    };

    class CommandPool2 : public VulkanObject
    {
    public:
        CommandPool2(VkCommandPool p_handler, const VulkanInstance::QueueInstance* p_queue)
                : _handler(p_handler), _queue(p_queue)
        {
            // ...
        }

        VkCommandPool GetHandler() const
        {
            return _handler;
        }
        
    private:
        const VkCommandPool _handler;

        const VulkanInstance::QueueInstance* _queue;
    };

    template <>
    class VulkanFactory::SubFactory<CommandPool2>
    {
    public:
        template <VulkanObjectKind T>
        static VulkanRef<CommandPool2> Create(const VulkanInstance::QueueInstance* p_queue,
            const CommandPoolFlag p_flags, VulkanRef<T> p_parent = nullptr)
        {
            VkCommandPoolCreateInfo createInfo { };
            createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            createInfo.flags = 0;
            createInfo.queueFamilyIndex = p_queue->FamilyIndex;

            if (p_flags & PROTECTED)
            {
                createInfo.flags |= VK_COMMAND_POOL_CREATE_PROTECTED_BIT;
            }
            if (p_flags & TRANSIENT)
            {
                createInfo.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
            }
            if (p_flags & RESET_COMMAND_BUFFER)
            {
                createInfo.flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            }

            VkCommandPool commandPool;
            vkCreateCommandPool(
                _bindFactory->_vulkanInstance.GetLogicalDevice(),
                &createInfo,
                nullptr,
                &commandPool);

            auto poolObject = new CommandPool2(commandPool, p_queue);
            
            return CreateObject(poolObject, Terminate, p_parent);
        }
        
    private:
        static void Terminate(VulkanObject* p_object)
        {
            auto poolObject = (CommandPool2*)p_object;
            
            vkDestroyCommandPool(
                _bindFactory->_vulkanInstance.GetLogicalDevice(),
                poolObject->GetHandler(),
                nullptr);

        }
    };
    
}
