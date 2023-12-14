#pragma once
#include "CommandPool.h"
#include "Controller/BaseVulkanController.h"

namespace DeepEngine::Renderer::Vulkan
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
        void OnTerminate() override { }

    private:
        VkCommandBuffer _commandBuffer = VK_NULL_HANDLE;

        const CommandPool* _commandPool;
        const bool _isSecondary;
    };
    
}
