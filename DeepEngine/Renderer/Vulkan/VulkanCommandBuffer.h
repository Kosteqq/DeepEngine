#pragma once
#include "VulkanCommandPool.h"
#include "Controller/BaseVulkanController.h"

namespace DeepEngine::Renderer::Vulkan
{

    class VulkanCommandBuffer final : public BaseVulkanController
    {
    public:
        VulkanCommandBuffer(VulkanCommandPool* p_commandPool, bool p_createAsSecondary);
        ~VulkanCommandBuffer() override = default;

        VkCommandBuffer GetVkCommandBuffer() const
        { return _commandBuffer; }

    protected:
        bool OnInitialize() override;
        void OnTerminate() override { }

    private:
        VkCommandBuffer _commandBuffer = VK_NULL_HANDLE;

        const VulkanCommandPool* _commandPool;
        const bool _isSecondary;
    };
    
}
