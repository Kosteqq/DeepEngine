#pragma once
#include "Vulkan/ShaderModule.h"
#include "Vulkan/GraphicsPipeline.h"


namespace DeepEngine::Renderer
{
    class TriangleRenderer
    {
    public:
        TriangleRenderer() = default;
        ~TriangleRenderer()
        {
            delete _graphicsPipeline;
        }
        
        bool Init(const char* p_vertexShaderPath, const char* p_fragmentShaderPath,
                const Vulkan::PipelineDynamicState& p_dynamicState, const Vulkan::PipelineColorBlend& p_colorBlend,
                const Vulkan::PipelineColorBlendAttachment& p_colorBlendAttachment,
                const Vulkan::PipelineRasterization& p_pipelineRasterization,
                Vulkan::PipelineLayout* p_pipelineLayout )
        {
            auto vertShader = new Vulkan::ShaderModule(p_vertexShaderPath, VK_SHADER_STAGE_VERTEX_BIT);
            if (!p_pipelineLayout->InitializeSubController(vertShader))
            {
                return false;
            }

            auto fragShader = new Vulkan::ShaderModule(p_fragmentShaderPath, VK_SHADER_STAGE_FRAGMENT_BIT);
            if (!p_pipelineLayout->InitializeSubController(fragShader))
            {
                return false;
            }

            _graphicsPipeline = new Vulkan::GraphicsPipeline(p_pipelineLayout, vertShader, fragShader, p_dynamicState,
                                                            p_colorBlend, {p_colorBlendAttachment},
                                                            p_pipelineRasterization);
            if(!p_pipelineLayout->InitializeSubController(_graphicsPipeline))
            {
                return false;
            }
            delete vertShader;
            delete fragShader;
            
            return true;    
        }

        const Vulkan::GraphicsPipeline* GetGraphicsPipeline() const
        {
            return _graphicsPipeline;
        }

    private:
        Vulkan::GraphicsPipeline* _graphicsPipeline;
        
    };
}

