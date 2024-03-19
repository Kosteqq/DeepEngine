#pragma once
#include "Controller/BaseVulkanController.h"
#include "Instance/VulkanInstance.h"
#include "PipelineLayout.h"
#include "ShaderModule.h"
#include "Factory.h"
#include "VulkanObject.h"

namespace DeepEngine::Engine::Renderer::Vulkan
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
    
    class GraphicsPipeline : public BaseVulkanController
    {
    public:
        GraphicsPipeline(PipelineLayout* p_pipelineLayout,
            const ShaderModule* p_vertShaderModule, const ShaderModule* p_fragShaderModule,
            const PipelineDynamicState& p_dynamicStateFlags, const PipelineColorBlend& p_colorBlend,
            const std::vector<PipelineColorBlendAttachment>& p_attachmentsBlend, const PipelineRasterization& p_rasterization);

        ~GraphicsPipeline() override = default;

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
        PipelineLayout* _pipelineLayout;
        
        const ShaderModule* _vertShaderModule; 
        const ShaderModule* _fragShaderModule;
        
        const PipelineDynamicState _dynamicStateFlags;
        const PipelineColorBlend _colorBlend;
        const std::vector<PipelineColorBlendAttachment> _attachemntsBlend;
        const PipelineRasterization _rasterization;
        
        VkPipeline _pipeline = VK_NULL_HANDLE;
    };

    class GraphicsPipeline2 : public VulkanObject
    {
    public:
        GraphicsPipeline2(VkPipeline p_handler) : _handler(p_handler)
        { }

        VkPipeline GetHandler() const
        { return _handler; }

    private:
        const VkPipeline _handler;
    };

    class GraphicsPipelineBuilder
    {
    public:
        struct DynamicStateDefinition
        {
            // For now, those two always have to be enabled
            bool Viewport: 1 = true;
            bool Scissor: 1 = true;

            bool LineWidth: 1 = false;
            // ...
        };

        struct ColorBlendDefinition
        {
            glm::vec4 ColorBlendConstants;
            bool EnableLogicalBlendOperation;
            VkLogicOp LogicalBlendOperation;
        };

        struct ColorBlendAttachmentDefinition
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

        struct RasterisationDefinition
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

    protected:
        virtual void GetShaderStages(Ref<ShaderModule2>& p_vertShader, Ref<ShaderModule2>& p_fragShader);
        virtual void DefineDynamicState(DynamicStateDefinition* p_definition) = 0;
        virtual void DefineColorBlend(ColorBlendDefinition* p_definition) = 0;
        virtual void DefineBlendAttachment(ColorBlendAttachmentDefinition* p_definition) = 0;
        virtual void DefineRasterisation(RasterisationDefinition* p_definition) = 0;
    };
    
    template<>
    class Factory::SubFactory<GraphicsPipeline2>
    {
    public:
        template <typename TBuilder>
        requires std::is_base_of_v<GraphicsPipelineBuilder, TBuilder>
        static Ref<GraphicsPipeline2> Create(
            const Ref<RenderPass2>& p_renderPass,
            const Ref<PipelineLayout2>& p_pipelineLayout,
            TBuilder& p_builder)
        {
            std::vector<VkPipelineShaderStageCreateInfo> stages;

            Ref<ShaderModule2> vertShader;
            Ref<ShaderModule2> fragShader;

            p_builder.GetShaderStages(vertShader, fragShader);
            InitializeShaderStages(vertShader, fragShader, &stages);

            GraphicsPipelineBuilder::DynamicStateDefinition dynamicStatesDefinition;
            p_builder.DefineDynamicState(&dynamicStatesDefinition);

            VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo { };
            InitializeDynamicStates(dynamicStatesDefinition, &dynamicStateCreateInfo);

            VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo { };
            InitializeVertexInputStage(&vertexInputCreateInfo);

            GraphicsPipelineBuilder::RasterisationDefinition rasterisaztionDefinition { };
            p_builder.DefineRasterisation(&rasterisaztionDefinition);
            
            VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo { };

            if (!InitializeRasterization(rasterisaztionDefinition, &rasterizerCreateInfo))
            {
                VULKAN_ERR("Failed to initialize graphics pipeline rasterisation");
                return nullptr;
            }

            // Get info about attachments
            std::vector<GraphicsPipelineBuilder::ColorBlendAttachmentDefinition> attachmentBlendDefinitions;
            p_builder.DefineBlendAttachment(&attachmentBlendDefinitions);

            GraphicsPipelineBuilder::ColorBlendDefinition colorBlendDefinition;
            p_builder.DefineColorBlend(&colorBlendDefinition);

            std::vector<VkPipelineColorBlendAttachmentState> attachmentsBlend(attachmentBlendDefinitions.size());
            VkPipelineColorBlendStateCreateInfo colorBlendingCreateInfo { };
            InitializeColorBlend(attachmentBlendDefinitions, colorBlendDefinition, &attachmentsBlend, &colorBlendingCreateInfo);

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
                
            pipelineInfo.pVertexInputState      = &vertexInputCreateInfo;
            pipelineInfo.pInputAssemblyState    = &inputAssemblyCreateInfo;
            pipelineInfo.pViewportState         = &viewportState;
            pipelineInfo.pRasterizationState    = &rasterizerCreateInfo;
            pipelineInfo.pMultisampleState      = &multisampleCreateInfo;
            pipelineInfo.pDepthStencilState     = nullptr; // optional
            pipelineInfo.pColorBlendState       = &colorBlendingCreateInfo;
            pipelineInfo.pDynamicState          = &dynamicStateCreateInfo;
            pipelineInfo.layout                 = p_pipelineLayout->GetHandler();

            pipelineInfo.renderPass             = p_renderPass->GetHandler();
            pipelineInfo.subpass                = p_pipelineLayout->GetSubPass().SubPassID;
                
            pipelineInfo.basePipelineHandle     = VK_NULL_HANDLE;
            pipelineInfo.basePipelineIndex      = -1;


            VkPipeline pipelineHandler;
            vkCreateGraphicsPipelines(
                _bindFactory->_vulkanInstance.GetLogicalDevice(),
                    VK_NULL_HANDLE,
                    1,
                    &pipelineInfo,
                    nullptr,
                    &pipelineHandler);

            return CreateObject(new GraphicsPipeline2(pipelineHandler), Terminate, p_pipelineLayout);
        }

        static void InitializeShaderStages(const Ref<ShaderModule2>& p_vertShader, const Ref<ShaderModule2>& p_fragShader,
            std::vector<VkPipelineShaderStageCreateInfo>* p_stages)
        {
            {
                VkPipelineShaderStageCreateInfo vertCreateInfo { };
                vertCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                vertCreateInfo.stage = p_vertShader->GetStageFlag();
                vertCreateInfo.module = p_vertShader->GetHandler();
                vertCreateInfo.pName = p_vertShader->GetFuncName();

                p_stages->push_back(vertCreateInfo);
            }

            {
                VkPipelineShaderStageCreateInfo fragCreateInfo { };
                fragCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                fragCreateInfo.stage = p_fragShader->GetStageFlag();
                fragCreateInfo.module = p_fragShader->GetHandler();
                fragCreateInfo.pName = p_fragShader->GetFuncName();
                    
                p_stages->push_back(fragCreateInfo);
            }
        }

        static void InitializeDynamicStates(const GraphicsPipelineBuilder::DynamicStateDefinition& p_stateDefinition,
            VkPipelineDynamicStateCreateInfo* p_createInfo)
        {
            std::vector<VkDynamicState> dynamicStates { };
            dynamicStates.reserve(16);
                
            if (p_stateDefinition.Viewport)
            {
                dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
            }
                
            if (p_stateDefinition.Scissor)
            {
                dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
            }
                
            if (p_stateDefinition.LineWidth)
            {
                dynamicStates.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);
            }

            p_createInfo->sType              = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            p_createInfo->dynamicStateCount  = static_cast<uint32_t>(dynamicStates.size());
            p_createInfo->pDynamicStates     = dynamicStates.data();
        }

        static void InitializeVertexInputStage(VkPipelineVertexInputStateCreateInfo* p_vertextInputCreateInfo)
        {
            p_vertextInputCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            p_vertextInputCreateInfo->vertexBindingDescriptionCount    = 0;
            p_vertextInputCreateInfo->pVertexBindingDescriptions       = nullptr;
            p_vertextInputCreateInfo->vertexAttributeDescriptionCount  = 0;
            p_vertextInputCreateInfo->pVertexAttributeDescriptions     = nullptr;
        }

        static bool InitializeRasterization(const GraphicsPipelineBuilder::RasterisationDefinition& p_rasterisationDefinition,
            VkPipelineRasterizationStateCreateInfo* p_rasterizerCreateInfo)
        {
            VulkanInstance& vulkanInstance = _bindFactory->_vulkanInstance;

            if (p_rasterisationDefinition.EnableDepthClamp)
            {
                if (!vulkanInstance.IsPhysicalExtensionEnabled(VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME))
                {
                    VULKAN_ERR("Failed to create graphics pipeline!!"
                               "\nReason: Tried to enable depth clamp with disabled \"{0}\" feature!",
                               VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME);
                    return false;
                }

                if (!vulkanInstance.GetPhysicalDeviceFeatures().depthClamp)
                {
                    VULKAN_ERR("Failed to create graphics pipeline!!"
                        "\nReason: Tried to enable depth clamp when your GPU does not support it!");
                    return false;
                }
            }

            float lineWidth = p_rasterisationDefinition.LineWidth;
            if (p_rasterisationDefinition.LineWidth > 1.0f)
            {
                if (!vulkanInstance.IsPhysicalExtensionEnabled(VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME))
                {
                    VULKAN_ERR("Failed to create pipeline!!"
                               "\nReason: Tried to enable Wide Lines with disabled \"{0}\" feature!",
                               VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME);
                    return false;
                }
                    
                if (!vulkanInstance.GetPhysicalDeviceFeatures().wideLines)
                {
                    VULKAN_ERR("Failed to create graphics pipeline!!"
                        "\nReason: Tried to enable Wide Lines when your GPU does not support it!");
                    return false;
                }

                auto& range = vulkanInstance.GetPhysicalDeviceProperties().limits.lineWidthRange;
                if (p_rasterisationDefinition.LineWidth < range[0] || p_rasterisationDefinition.LineWidth > range[1])
                {
                    VULKAN_WARN("Passed LineWidth ({}) is outside GPU line width range limit ({} - {})!",
                                lineWidth, range[0], range[1]);
                    lineWidth = glm::clamp(lineWidth, range[0], range[1]);
                }
            }

            {
                float lineGranularity = vulkanInstance.GetPhysicalDeviceProperties().limits.lineWidthGranularity;
                lineWidth = glm::round(lineWidth / lineGranularity) * lineGranularity;
            }
                
            p_rasterizerCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            p_rasterizerCreateInfo->depthClampEnable           = p_rasterisationDefinition.EnableDepthClamp;
            p_rasterizerCreateInfo->rasterizerDiscardEnable    = p_rasterisationDefinition.EnableDiscardRasterizer;
            p_rasterizerCreateInfo->polygonMode                = p_rasterisationDefinition.PolygonMode;
            p_rasterizerCreateInfo->lineWidth                  = lineWidth;
            p_rasterizerCreateInfo->cullMode                   = p_rasterisationDefinition.CullMode;
            p_rasterizerCreateInfo->frontFace                  = p_rasterisationDefinition.FrontFace;

            p_rasterizerCreateInfo->depthClampEnable           = p_rasterisationDefinition.EnableDepthBias;
            p_rasterizerCreateInfo->depthBiasConstantFactor    = p_rasterisationDefinition.DepthBiasConstFactor;
            p_rasterizerCreateInfo->depthBiasClamp             = p_rasterisationDefinition.DepthBiasClamp;
            p_rasterizerCreateInfo->depthBiasSlopeFactor       = p_rasterisationDefinition.DepthBiasSlopeFactor;

            return true;
        }

        static void InitializeColorBlend(
            const std::vector<GraphicsPipelineBuilder::ColorBlendAttachmentDefinition>& p_attachemntsBlendDefinition,
            const GraphicsPipelineBuilder::ColorBlendDefinition& p_colorBlendDefinition,
            std::vector<VkPipelineColorBlendAttachmentState>* p_attachmentsBlends,
            VkPipelineColorBlendStateCreateInfo* p_colorBlendCreateInfo)
        {
            for (uint32_t i = 0; i < p_attachmentsBlends->size(); i++)
            {
                VkPipelineColorBlendAttachmentState& attachmentBlend = (*p_attachmentsBlends)[i];
                attachmentBlend = { };
                attachmentBlend.colorWriteMask = 0;
                attachmentBlend.colorWriteMask |= VK_COLOR_COMPONENT_R_BIT * p_attachemntsBlendDefinition[i].WriteChannelR;
                attachmentBlend.colorWriteMask |= VK_COLOR_COMPONENT_G_BIT * p_attachemntsBlendDefinition[i].WriteChannelG;
                attachmentBlend.colorWriteMask |= VK_COLOR_COMPONENT_B_BIT * p_attachemntsBlendDefinition[i].WriteChannelB;
                attachmentBlend.colorWriteMask |= VK_COLOR_COMPONENT_A_BIT * p_attachemntsBlendDefinition[i].WriteChannelA;

                attachmentBlend.blendEnable         = p_attachemntsBlendDefinition[i].EnableBlend;
                    
                attachmentBlend.colorBlendOp        = p_attachemntsBlendDefinition[i].ColorBlendOperation;
                attachmentBlend.srcColorBlendFactor = p_attachemntsBlendDefinition[i].SrsColorBlendFactor;
                attachmentBlend.dstColorBlendFactor = p_attachemntsBlendDefinition[i].DstColorBlendFactor;
                    
                attachmentBlend.alphaBlendOp        = p_attachemntsBlendDefinition[i].AlphaBlendOperation;
                attachmentBlend.srcAlphaBlendFactor = p_attachemntsBlendDefinition[i].SrsAlphaBlendFactor;
                attachmentBlend.dstAlphaBlendFactor = p_attachemntsBlendDefinition[i].DstAlphaBlendFactor;
            }

            p_colorBlendCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            p_colorBlendCreateInfo->logicOpEnable       = p_colorBlendDefinition.EnableLogicalBlendOperation;
            p_colorBlendCreateInfo->logicOp             = p_colorBlendDefinition.LogicalBlendOperation;
            p_colorBlendCreateInfo->attachmentCount     = static_cast<uint32_t>(p_attachmentsBlends->size());
            p_colorBlendCreateInfo->pAttachments        = p_attachmentsBlends->data();
            p_colorBlendCreateInfo->blendConstants[0]   = p_colorBlendDefinition.ColorBlendConstants.r;
            p_colorBlendCreateInfo->blendConstants[1]   = p_colorBlendDefinition.ColorBlendConstants.g;
            p_colorBlendCreateInfo->blendConstants[2]   = p_colorBlendDefinition.ColorBlendConstants.b;
            p_colorBlendCreateInfo->blendConstants[3]   = p_colorBlendDefinition.ColorBlendConstants.a;
        }

        static void Terminate(VulkanObject* p_object)
        {
            vkDestroyPipeline(
                _bindFactory->_vulkanInstance.GetLogicalDevice(),
                ((GraphicsPipeline2*)p_object)->GetHandler(),
                nullptr);
        }
    };
    
}
