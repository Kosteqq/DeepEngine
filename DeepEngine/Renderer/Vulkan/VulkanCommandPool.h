#pragma once
#include "Controller/BaseVulkanController.h"
#include "Instance/VulkanInstance.h"

namespace DeepEngine::Renderer::Vulkan
{

    enum CommandPoolFlag
    {
        TRANSIENT = 1 << 0,
        RESET_COMMAND_BUFFER = 1 << 1,
        PROTECTED = 1 << 2,
    };

    class VulkanCommandPool final : public BaseVulkanController
    {
    public:
        VulkanCommandPool(const VulkanInstance::QueueInstance* p_queue, const CommandPoolFlag p_flags);
        ~VulkanCommandPool() override = default;

        VkCommandPool GetVkCommandPool() const
        { return _commandPool; }

    protected:
        bool OnInitialize() override;
        void OnTerminate() override;

    private:
        VkCommandPool _commandPool;

        const VulkanInstance::QueueInstance* _queue;
        const CommandPoolFlag _flag;
    };
    
}
