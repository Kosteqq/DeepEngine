#pragma once
#include "Controller/BaseVulkanController.h"
#include "VulkanRenderPass.h"

namespace DeepEngine::Renderer::Vulkan
{

    class VulkanRenderPass;

    class VulkanPipelineLayout : public BaseVulkanController
    {
    public:
        VulkanPipelineLayout(VulkanRenderPass* p_renderPass, uint32_t p_subPassIndex);
        ~VulkanPipelineLayout() override = default;

        VulkanRenderPass* GetRenderPass() const
        { return _renderPass; }

        uint32_t GetSubPassIndex() const
        { return _subPassIndex; }

        VkPipelineLayout GetVkPipelineLayout() const
        { return _pipelineLayout; }

    protected:
        bool OnInitialize() final;
        void OnTerminate() final;

    protected:
        VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
        const uint32_t _subPassIndex;
        VulkanRenderPass* _renderPass;
    };
    
}
