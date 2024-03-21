#pragma once
#include "Vulkan/ShaderModule.h"
#include "Vulkan/GraphicsPipeline.h"


namespace DeepEngine::Engine::Renderer
{
    class TriangleGraphicsPipelineBuilder : public Vulkan::GraphicsPipelineBuilder
    {
    public:
        TriangleGraphicsPipelineBuilder(const std::string& p_vertexShaderPath, const std::string& p_fragmentShaderPath)
            : _vertexShaderPath(p_vertexShaderPath), _fragmentShaderPath(p_fragmentShaderPath)
        {
            
        }
        
    protected:
        void GetShaderStages(
            Vulkan::Ref<Vulkan::ShaderModule2>& p_vertShader,
            Vulkan::Ref<Vulkan::ShaderModule2>& p_fragShader) override
        {
            p_vertShader = Vulkan::Factory::SubFactory<Vulkan::ShaderModule2>::Create(_vertexShaderPath, VK_SHADER_STAGE_VERTEX_BIT);
            p_fragShader = Vulkan::Factory::SubFactory<Vulkan::ShaderModule2>::Create(_fragmentShaderPath, VK_SHADER_STAGE_FRAGMENT_BIT);
        }
        
        void DefineDynamicState(DynamicStateDefinition& p_definition) override
        {
            p_definition.Viewport = true;
            p_definition.Scissor = true;
            p_definition.LineWidth = false;
        }
        
        void DefineColorBlend(ColorBlendDefinition& p_definition) override
        {
            p_definition.ColorBlendConstants = { 1.0f, 1.0f, 1.0f, 1.0f };
            p_definition.EnableLogicalBlendOperation = false;
        }
        
        void DefineBlendAttachment(uint64_t p_attachmentIndex, ColorBlendAttachmentDefinition& p_definition) override
        {
            p_definition.WriteChannelR = true;
            p_definition.WriteChannelG = true;
            p_definition.WriteChannelB = true;
            p_definition.WriteChannelA = true;
            p_definition.EnableBlend = false;
        }
        
        void DefineRasterisation(RasterisationDefinition& p_definition) override
        {
            p_definition.EnableDepthClamp = false;
            p_definition.EnableDiscardRasterizer = false;
            p_definition.EnableDepthBias = false;
            p_definition.PolygonMode = VK_POLYGON_MODE_FILL;
            p_definition.CullMode = VK_CULL_MODE_NONE;
            p_definition.FrontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        }

    private:
        std::string _vertexShaderPath;
        std::string _fragmentShaderPath;
    };

    
    class TriangleRenderer
    {
    public:
        TriangleRenderer() = default;
        
        bool Init(const std::string& p_vertexShaderPath, const std::string& p_fragmentShaderPath,
            const Vulkan::Ref<Vulkan::RenderPass2> p_renderPass,
            const Vulkan::Ref<Vulkan::PipelineLayout2>& p_pipelineLayout)
        {
            TriangleGraphicsPipelineBuilder builder(p_vertexShaderPath, p_fragmentShaderPath);
            _graphicsPipeline = Vulkan::Factory::SubFactory<Vulkan::GraphicsPipeline2>::Create(p_renderPass, p_pipelineLayout, &builder);
            
            return _graphicsPipeline != nullptr;    
        }

        Vulkan::Ref<Vulkan::GraphicsPipeline2> GetGraphicsPipeline() const
        { return _graphicsPipeline; }

    private:
        Vulkan::Ref<Vulkan::GraphicsPipeline2> _graphicsPipeline;
    };
}

