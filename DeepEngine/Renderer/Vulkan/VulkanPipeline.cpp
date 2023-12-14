#include "VulkanPipeline.h"

namespace DeepEngine::Renderer::Vulkan
{

    VulkanPipeline::VulkanPipeline(VulkanPipelineLayout* p_pipelineLayout,
        const VulkanShaderModule* p_vertShaderModule, const VulkanShaderModule* p_fragShaderModule,
        const PipelineDynamicState& p_dynamicStateFlags, const PipelineColorBlend& p_colorBlend,
        const std::vector<PipelineColorBlendAttachment>& p_attachmentsBlend, const PipelineRasterization& p_rasterization)
        : _pipelineLayout(p_pipelineLayout), 
        _vertShaderModule(p_vertShaderModule), _fragShaderModule(p_fragShaderModule),
        _dynamicStateFlags(p_dynamicStateFlags), _colorBlend(p_colorBlend), _attachemntsBlend(p_attachmentsBlend),
        _rasterization(p_rasterization)
    { }
    
    bool VulkanPipeline::OnInitialize()
    {
        std::vector<VkPipelineShaderStageCreateInfo> stages;

        {
            VkPipelineShaderStageCreateInfo vertCreateInfo { };
            vertCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertCreateInfo.stage = _vertShaderModule->GetShaderStageFlags();
            vertCreateInfo.module = _vertShaderModule->GetShaderModule();
            vertCreateInfo.pName = _vertShaderModule->GetShaderFuncName().c_str();

            stages.push_back(vertCreateInfo);
        }

        {
            VkPipelineShaderStageCreateInfo fragCreateInfo { };
            fragCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragCreateInfo.stage = _fragShaderModule->GetShaderStageFlags();
            fragCreateInfo.module = _fragShaderModule->GetShaderModule();
            fragCreateInfo.pName = _fragShaderModule->GetShaderFuncName().c_str();
                
            stages.push_back(fragCreateInfo);
        }

        std::vector<VkDynamicState> dynamicStates { };
        dynamicStates.reserve(16);
            
        if (_dynamicStateFlags.Viewport)
        {
            dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
        }
            
        if (_dynamicStateFlags.Scissor)
        {
            dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
        }
            
        if (_dynamicStateFlags.LineWidth)
        {
            dynamicStates.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);
        }

        VkPipelineDynamicStateCreateInfo dynamicState { };
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        // No fucking idea how pass this info
        VkPipelineVertexInputStateCreateInfo vertexBufferCreateInfo { };
        vertexBufferCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexBufferCreateInfo.vertexBindingDescriptionCount    = 0;
        vertexBufferCreateInfo.pVertexBindingDescriptions       = nullptr;
        vertexBufferCreateInfo.vertexAttributeDescriptionCount  = 0;
        vertexBufferCreateInfo.pVertexAttributeDescriptions     = nullptr;

        VulkanInstance* vulkanInstance = GetVulkanInstanceController();

        if (_rasterization.EnableDepthClamp)
        {
            if (!vulkanInstance->IsPhysicalExtensionEnabled(VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME))
            {
                VULKAN_ERR("Failed to create graphics pipeline!!"
                           "\nReason: Tried to enable depth clamp with disabled \"{0}\" feature!",
                           VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME);
                return false;
            }

            if (!vulkanInstance->GetPhysicalDeviceFeatures().depthClamp)
            {
                VULKAN_ERR("Failed to create graphics pipeline!!"
                    "\nReason: Tried to enable depth clamp when your GPU does not support it!");
                return false;
            }
        }

        float lineWidth = _rasterization.LineWidth;
        if (_rasterization.LineWidth > 1.0f)
        {
            if (!vulkanInstance->IsPhysicalExtensionEnabled(VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME))
            {
                VULKAN_ERR("Failed to create pipeline!!"
                           "\nReason: Tried to enable Wide Lines with disabled \"{0}\" feature!",
                           VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME);
                return false;
            }
                
            if (!vulkanInstance->GetPhysicalDeviceFeatures().wideLines)
            {
                VULKAN_ERR("Failed to create graphics pipeline!!"
                    "\nReason: Tried to enable Wide Lines when your GPU does not support it!");
                return false;
            }

            auto& range = vulkanInstance->GetPhysicalDeviceProperties().limits.lineWidthRange;
            if (_rasterization.LineWidth < range[0] || _rasterization.LineWidth > range[1])
            {
                VULKAN_WARN("Passed LineWidth ({}) is outside GPU line width range limit ({} - {})!",
                            lineWidth, range[0], range[1]);
                lineWidth = glm::clamp(lineWidth, range[0], range[1]);
            }
        }

        {
            float lineGranularity = vulkanInstance->GetPhysicalDeviceProperties().limits.lineWidthGranularity;
            lineWidth = glm::round(lineWidth / lineGranularity) * lineGranularity;
        }
            
        VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo { };
        rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizerCreateInfo.depthClampEnable           = _rasterization.EnableDepthClamp;
        rasterizerCreateInfo.rasterizerDiscardEnable    = _rasterization.EnableDiscardRasterizer;
        rasterizerCreateInfo.polygonMode                = _rasterization.PolygonMode;
        rasterizerCreateInfo.lineWidth                  = lineWidth;
        rasterizerCreateInfo.cullMode                   = _rasterization.CullMode;
        rasterizerCreateInfo.frontFace                  = _rasterization.FrontFace;

        rasterizerCreateInfo.depthClampEnable           = _rasterization.EnableDepthBias;
        rasterizerCreateInfo.depthBiasConstantFactor    = _rasterization.DepthBiasConstFactor;
        rasterizerCreateInfo.depthBiasClamp             = _rasterization.DepthBiasClamp;
        rasterizerCreateInfo.depthBiasSlopeFactor       = _rasterization.DepthBiasSlopeFactor;

        std::vector<VkPipelineColorBlendAttachmentState> attachmentsBlend(_attachemntsBlend.size());
        for (uint32_t i = 0; i < _attachemntsBlend.size(); i++)
        {
            VkPipelineColorBlendAttachmentState& attachmentBlend = attachmentsBlend[i];
            attachmentBlend = { };
            attachmentBlend.colorWriteMask = 0;
            attachmentBlend.colorWriteMask |= VK_COLOR_COMPONENT_R_BIT * _attachemntsBlend[i].WriteChannelR;
            attachmentBlend.colorWriteMask |= VK_COLOR_COMPONENT_G_BIT * _attachemntsBlend[i].WriteChannelG;
            attachmentBlend.colorWriteMask |= VK_COLOR_COMPONENT_B_BIT * _attachemntsBlend[i].WriteChannelB;
            attachmentBlend.colorWriteMask |= VK_COLOR_COMPONENT_A_BIT * _attachemntsBlend[i].WriteChannelA;

            attachmentBlend.blendEnable         = _attachemntsBlend[i].EnableBlend;
                
            attachmentBlend.colorBlendOp        = _attachemntsBlend[i].ColorBlendOperation;
            attachmentBlend.srcColorBlendFactor = _attachemntsBlend[i].SrsColorBlendFactor;
            attachmentBlend.dstColorBlendFactor = _attachemntsBlend[i].DstColorBlendFactor;
                
            attachmentBlend.alphaBlendOp        = _attachemntsBlend[i].AlphaBlendOperation;
            attachmentBlend.srcAlphaBlendFactor = _attachemntsBlend[i].SrsAlphaBlendFactor;
            attachmentBlend.dstAlphaBlendFactor = _attachemntsBlend[i].DstAlphaBlendFactor;
        }

        VkPipelineColorBlendStateCreateInfo colorBlendingCreateInfo { };
        colorBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendingCreateInfo.logicOpEnable       = _colorBlend.EnableLogicalBlendOperation;
        colorBlendingCreateInfo.logicOp             = _colorBlend.LogicalBlendOperation;
        colorBlendingCreateInfo.attachmentCount     = static_cast<uint32_t>(attachmentsBlend.size());
        colorBlendingCreateInfo.pAttachments        = attachmentsBlend.data();
        colorBlendingCreateInfo.blendConstants[0]   = _colorBlend.ColorBlendConstants.r;
        colorBlendingCreateInfo.blendConstants[1]   = _colorBlend.ColorBlendConstants.g;
        colorBlendingCreateInfo.blendConstants[2]   = _colorBlend.ColorBlendConstants.b;
        colorBlendingCreateInfo.blendConstants[3]   = _colorBlend.ColorBlendConstants.a;

        // Hardcoded
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo { };
        inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

        // Hardcoded
        VkPipelineMultisampleStateCreateInfo multisampleCreateInfo { };
        multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleCreateInfo.sampleShadingEnable   = VK_FALSE;
        multisampleCreateInfo.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
        multisampleCreateInfo.minSampleShading      = 1.0;
        multisampleCreateInfo.pSampleMask           = nullptr;
        multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
        multisampleCreateInfo.alphaToOneEnable      = VK_FALSE;

        
        VkViewport viewport;
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = 800;
        viewport.height = 600;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
            
        VkRect2D scissor;
        scissor.offset = {0, 0};
        scissor.extent = VkExtent2D(800, 600);
        
        VkPipelineViewportStateCreateInfo viewportState { };
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;
            
        VkGraphicsPipelineCreateInfo pipelineInfo { };
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = static_cast<uint32_t>(stages.size());
        pipelineInfo.pStages = stages.data();
            
        pipelineInfo.pVertexInputState      = &vertexBufferCreateInfo;
        pipelineInfo.pInputAssemblyState    = &inputAssemblyCreateInfo;
        pipelineInfo.pViewportState         = &viewportState;
        pipelineInfo.pRasterizationState    = &rasterizerCreateInfo;
        pipelineInfo.pMultisampleState      = &multisampleCreateInfo;
        pipelineInfo.pDepthStencilState     = nullptr; // optional
        pipelineInfo.pColorBlendState       = &colorBlendingCreateInfo;
        pipelineInfo.pDynamicState          = &dynamicState;
        pipelineInfo.layout                 = _pipelineLayout->GetVkPipelineLayout();

        pipelineInfo.renderPass             = _pipelineLayout->GetRenderPass()->GetVkRenderPass();
        pipelineInfo.subpass                = _pipelineLayout->GetSubPassIndex();
            
        pipelineInfo.basePipelineHandle     = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex      = -1;

        VULKAN_CHECK_CREATE(vkCreateGraphicsPipelines(
            GetVulkanInstanceController()->GetLogicalDevice(),
                VK_NULL_HANDLE,
                1,
                &pipelineInfo,
                nullptr,
                &_pipeline),
            "Failed to create graphics pipeline!")
            
        return true;
    }

    void VulkanPipeline::OnTerminate()
    {
        vkDestroyPipeline(GetVulkanInstanceController()->GetLogicalDevice(), _pipeline, nullptr);
    }
}
