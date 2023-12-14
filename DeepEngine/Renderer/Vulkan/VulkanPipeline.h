#pragma once
#include "Controller/BaseVulkanController.h"
#include "Instance/VulkanInstance.h"
#include "VulkanPipelineLayout.h"
#include "VulkanShaderModule.h"

namespace DeepEngine::Renderer::Vulkan
{
    struct PipelineDynamicState
    {
        // For now, those two always have to be enabled
        bool Viewport: 1 = true;
        bool Scissor: 1 = true;

        bool LineWidth: 1 = false;
        // ...
    };

    struct PipelineColorBlend
    {
        glm::vec4 ColorBlendConstants;
        bool EnableLogicalBlendOperation;
        VkLogicOp LogicalBlendOperation;
    };

    struct PipelineColorBlendAttachment
    {
        bool WriteChannelR: 1 = true;
        bool WriteChannelG: 1 = true;
        bool WriteChannelB: 1 = true;
        bool WriteChannelA: 1 = true;
        bool EnableBlend: 1;

        VkBlendOp ColorBlendOperation       = VK_BLEND_OP_ADD;
        VkBlendFactor SrsColorBlendFactor   = VK_BLEND_FACTOR_ONE;
        VkBlendFactor DstColorBlendFactor   = VK_BLEND_FACTOR_ONE;

        VkBlendOp AlphaBlendOperation       = VK_BLEND_OP_ADD;
        VkBlendFactor SrsAlphaBlendFactor   = VK_BLEND_FACTOR_ONE;
        VkBlendFactor DstAlphaBlendFactor   = VK_BLEND_FACTOR_ONE;
    };

    struct PipelineRasterization
    {
        bool EnableDepthClamp: 1        = false;
        bool EnableDiscardRasterizer: 1 = false;
        bool EnableDepthBias: 1         = false;

        VkPolygonMode PolygonMode       = VK_POLYGON_MODE_FILL;
        float LineWidth                 = 1.0f;
        VkCullModeFlags CullMode        = VK_CULL_MODE_NONE;
        VkFrontFace FrontFace           = VK_FRONT_FACE_COUNTER_CLOCKWISE;

        float DepthBiasConstFactor      = 0.0f;
        float DepthBiasClamp            = 0.0f;
        float DepthBiasSlopeFactor      = 0.0f;
    };
    
    class VulkanPipeline : public BaseVulkanController
    {
    public:
        VulkanPipeline(VulkanPipelineLayout* p_pipelineLayout,
            const VulkanShaderModule* p_vertShaderModule, const VulkanShaderModule* p_fragShaderModule,
            const PipelineDynamicState& p_dynamicStateFlags, const PipelineColorBlend& p_colorBlend,
            const std::vector<PipelineColorBlendAttachment>& p_attachmentsBlend, const PipelineRasterization& p_rasterization);

        ~VulkanPipeline() override = default;

        const VkRenderPass& GetVkRenderPass() const
        { return _pipelineLayout->GetRenderPass()->GetVkRenderPass(); }

        const VkPipelineLayout& GetVkPipelineLayout() const
        { return _pipelineLayout->GetVkPipelineLayout(); }
        
        const VkPipeline& GetVkPipeline() const
        { return _pipeline; }

    protected:
        bool OnInitialize() final;
        void OnTerminate() final;

    private:
        VulkanPipelineLayout* _pipelineLayout;
        
        const VulkanShaderModule* _vertShaderModule; 
        const VulkanShaderModule* _fragShaderModule;
        
        const PipelineDynamicState _dynamicStateFlags;
        const PipelineColorBlend _colorBlend;
        const std::vector<PipelineColorBlendAttachment> _attachemntsBlend;
        const PipelineRasterization _rasterization;
        
        VkPipeline _pipeline = VK_NULL_HANDLE;
    };
    
}
