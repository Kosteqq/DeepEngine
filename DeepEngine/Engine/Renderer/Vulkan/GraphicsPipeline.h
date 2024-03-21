#pragma once
#include "Controller/BaseVulkanController.h"
#include "Instance/VulkanInstance.h"
#include "PipelineLayout.h"
#include "ShaderModule.h"
#include "Factory.h"
#include "VulkanObject.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{
    
    class GraphicsPipeline : public VulkanObject
    {
    public:
        GraphicsPipeline(VkPipeline p_handler) : _handler(p_handler)
        { }

        VkPipeline GetHandler() const
        { return _handler; }

    private:
        const VkPipeline _handler;
    };

    class GraphicsPipelineBuilder
    {
        friend class Factory::SubFactory<GraphicsPipeline>;
        
    protected:
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
        
    public:
        virtual ~GraphicsPipelineBuilder() = default;

    private:
        virtual void GetShaderStages(Ref<ShaderModule>& p_vertShader, Ref<ShaderModule>& p_fragShader) = 0;
        virtual void DefineDynamicState(DynamicStateDefinition& p_definition) = 0;
        virtual void DefineColorBlend(ColorBlendDefinition& p_definition) = 0;
        virtual void DefineBlendAttachment(uint64_t p_attachmentIndex, ColorBlendAttachmentDefinition& p_definition) = 0;
        virtual void DefineRasterisation(RasterisationDefinition& p_definition) = 0;
    };
    
    template<>
    class Factory::SubFactory<GraphicsPipeline>
    {
    public:
        static Ref<GraphicsPipeline> Create(
            const Ref<RenderPass>& p_renderPass,
            const Ref<PipelineLayout>& p_pipelineLayout,
            GraphicsPipelineBuilder* p_builder);

    private:
        static void InitializeShaderStages(const Ref<ShaderModule>& p_vertShader, const Ref<ShaderModule>& p_fragShader,
                                           std::vector<VkPipelineShaderStageCreateInfo>* p_stages);
        static void InitializeDynamicStates(const GraphicsPipelineBuilder::DynamicStateDefinition& p_stateDefinition,
                                            std::vector<VkDynamicState>& p_dynamicStates);
        static void InitializeVertexInputStage(VkPipelineVertexInputStateCreateInfo* p_vertexInputCreateInfo);
        static bool InitializeRasterization(const GraphicsPipelineBuilder::RasterisationDefinition& p_rasterisationDefinition,
                                            VkPipelineRasterizationStateCreateInfo* p_rasterizerCreateInfo);
        static void InitializeColorBlend(
            const std::vector<GraphicsPipelineBuilder::ColorBlendAttachmentDefinition>& p_attachemntsBlendDefinition,
            const GraphicsPipelineBuilder::ColorBlendDefinition& p_colorBlendDefinition,
            std::vector<VkPipelineColorBlendAttachmentState>* p_attachmentsBlends,
            VkPipelineColorBlendStateCreateInfo* p_colorBlendCreateInfo);

        static void Terminate(VulkanObject* p_object);
    };
    
}
