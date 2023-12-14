#pragma once
#include "Controller/BaseVulkanController.h"
#include "RenderPass.h"

namespace DeepEngine::Renderer::Vulkan
{

    class RenderPass;

    class PipelineLayout : public BaseVulkanController
    {
    public:
        PipelineLayout(RenderPass* p_renderPass, uint32_t p_subPassIndex);
        ~PipelineLayout() override = default;

        RenderPass* GetRenderPass() const
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
        RenderPass* _renderPass;
    };
    
}
