#pragma once
#include "Controller/BaseVulkanController.h"
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{

#ifndef _VULKAN_COMMAND_BUFFER_CLASS_DEFINED
    class CommandBuffer;
#endif

    enum CommandPoolFlag
    {
        TRANSIENT = 1 << 0,
        RESET_COMMAND_BUFFER = 1 << 1,
        PROTECTED = 1 << 2,
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
    
}
