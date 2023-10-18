#pragma once
#include <fstream>
#include <memory>
#define NOMINMAX
#include <vulkan/vulkan.h>

#include "VulkanLogicalLayer.h"
#include "VulkanSwapChain.h"
#include "VulkanRenderPass.h"
#include "Architecture/EngineSystem.h"
#include "Debugs/Logger.h"

namespace DeepEngine::Renderer
{

    class VulkanPipeline
    {
    public:
        VulkanPipeline(std::shared_ptr<Debug::Logger> p_logger, const VulkanLogicalLayer* p_logicalLayer,
            const VulkanSwapChain* p_swapchain, const VulkanRenderPass* p_renderPass)
            : _logger(p_logger), _logicalLayer(p_logicalLayer), _swapchain(p_swapchain), _renderPass(p_renderPass)
        { }

        ~VulkanPipeline()
        {
            Terminate();
        }

        bool Initialize()
        {
            VkShaderModule vertModule;
            CreateShaderModule("../DeepEngine/VulkanPrototype/Instance/Shader/vert.spv", &vertModule);
            
            VkShaderModule fragModule;
            CreateShaderModule("../DeepEngine/VulkanPrototype/Instance/Shader/frag.spv", &fragModule);

            VkPipelineShaderStageCreateInfo vertCreateInfo { };
            vertCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
            vertCreateInfo.module = vertModule;
            vertCreateInfo.pName = "main"; 

            VkPipelineShaderStageCreateInfo fragCreateInfo { };
            fragCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragCreateInfo.module = fragModule;
            fragCreateInfo.pName = "main";

            std::vector<VkDynamicState> dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
            };
            VkPipelineDynamicStateCreateInfo dynamicState { };
            dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
            dynamicState.pDynamicStates = dynamicStates.data();

            VkPipelineVertexInputStateCreateInfo vertexBufferCreateInfo { };
            vertexBufferCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexBufferCreateInfo.vertexBindingDescriptionCount = 0;
            vertexBufferCreateInfo.pVertexBindingDescriptions = nullptr;
            vertexBufferCreateInfo.vertexAttributeDescriptionCount = 0;
            vertexBufferCreateInfo.pVertexAttributeDescriptions = nullptr;

            VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo { };
            inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

            VkViewport viewport { };
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float)_swapchain->GetExtent().width;
            viewport.height = (float)_swapchain->GetExtent().height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            VkRect2D scissor { };
            scissor.offset = {0, 0};
            scissor.extent = _swapchain->GetExtent();

            VkPipelineViewportStateCreateInfo viewportState { };
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.pViewports = &viewport;
            viewportState.scissorCount = 1;
            viewportState.pScissors = &scissor;

            VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo { };
            rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizerCreateInfo.depthClampEnable = VK_FALSE;           // require GPU feature
            rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
            rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;    // other require enable feature
            rasterizerCreateInfo.lineWidth = 1.0f;                      // thicker line require wideLines feature
            rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

            // Optional... (for shadow mapping)
            rasterizerCreateInfo.depthClampEnable = VK_FALSE;
            rasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
            rasterizerCreateInfo.depthBiasClamp = 0.0f;
            rasterizerCreateInfo.depthBiasSlopeFactor = 0.0f;

            VkPipelineMultisampleStateCreateInfo multisampleCreateInfo { };
            multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
            multisampleCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            // Optional...
            multisampleCreateInfo.minSampleShading = 1.0;
            multisampleCreateInfo.pSampleMask = nullptr;
            multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
            multisampleCreateInfo.alphaToOneEnable = VK_FALSE;

            VkPipelineColorBlendAttachmentState colorBlendAttachment { };
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;
            // Optional...
            //colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
            //colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            //colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
            //colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            //colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            //colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

            VkPipelineColorBlendStateCreateInfo colorBlendingCreateInfo { };
            colorBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlendingCreateInfo.logicOpEnable = VK_FALSE;
            colorBlendingCreateInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
            colorBlendingCreateInfo.attachmentCount = 1;
            colorBlendingCreateInfo.pAttachments = &colorBlendAttachment;
            colorBlendingCreateInfo.blendConstants[0] = 0.0f;
            colorBlendingCreateInfo.blendConstants[1] = 0.0f;
            colorBlendingCreateInfo.blendConstants[2] = 0.0f;
            colorBlendingCreateInfo.blendConstants[3] = 0.0f;

            VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo { };
            pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutCreateInfo.setLayoutCount = 0;
            pipelineLayoutCreateInfo.pSetLayouts = nullptr;
            pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
            pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

            const auto result = vkCreatePipelineLayout(_logicalLayer->GetLogicalDevice(), &pipelineLayoutCreateInfo,
            nullptr, &_pipelineLayout);

            if (result != VK_SUCCESS)
            {
                vkDestroyShaderModule(_logicalLayer->GetLogicalDevice(), vertModule, nullptr);
                vkDestroyShaderModule(_logicalLayer->GetLogicalDevice(), fragModule, nullptr);
                LOG_ERR(_logger, "Failed to create pipeline layout with returned result {}", string_VkResult(result));
                return false;
            }

            const std::vector<VkPipelineShaderStageCreateInfo> stages = {
                vertCreateInfo,
                fragCreateInfo,
            };
            
            VkGraphicsPipelineCreateInfo pipelineInfo { };
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = 2;
            pipelineInfo.pStages = stages.data();

            pipelineInfo.pVertexInputState = &vertexBufferCreateInfo;
            pipelineInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizerCreateInfo;
            pipelineInfo.pMultisampleState = &multisampleCreateInfo;
            pipelineInfo.pDepthStencilState = nullptr; // optional
            pipelineInfo.pColorBlendState = &colorBlendingCreateInfo;
            pipelineInfo.pDynamicState = &dynamicState;
            
            pipelineInfo.layout = _pipelineLayout;
            
            pipelineInfo.renderPass = _renderPass->GetVulkanRenderPass();
            pipelineInfo.subpass = 0;

            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
            pipelineInfo.basePipelineIndex = -1;

            const auto result2 = vkCreateGraphicsPipelines(_logicalLayer->GetLogicalDevice(),
                VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline);
            
            vkDestroyShaderModule(_logicalLayer->GetLogicalDevice(), vertModule, nullptr);
            vkDestroyShaderModule(_logicalLayer->GetLogicalDevice(), fragModule, nullptr);

            if (result2 != VK_SUCCESS)
            {
                LOG_ERR(_logger, "Failed to create pipeline with returned result {}", string_VkResult(result));
                return false;
            }

            LOG_INFO(_logger, "Created vulkan pipeline");

            _initialized = true;
            return true;
        }

        void Terminate()
        {
            if (_initialized)
            {
                vkDestroyPipeline(_logicalLayer->GetLogicalDevice(), _pipeline, nullptr);
                vkDestroyPipelineLayout(_logicalLayer->GetLogicalDevice(), _pipelineLayout, nullptr);
                _initialized = false;
            }
        }

        VkPipeline GetPipeline() const
        { return _pipeline; }
        
    private:
        std::vector<char> ReadFile(const std::string& p_filename)
        {
            std::ifstream file(p_filename, std::ios::ate | std::ios::binary);

            if (!file.is_open())
            {
                return std::vector<char> { };
            }

            size_t fileSize = file.tellg();
            std::vector<char> buffer(fileSize);

            file.seekg(0);
            file.read(buffer.data(), fileSize);
            file.close();

            return buffer;
        }

        bool CreateShaderModule(const char* p_filepath, VkShaderModule* p_shaderModule)
        {
            auto shaderCode = ReadFile(p_filepath);

            if (shaderCode.size() == 0)
            {
                LOG_ERR(_logger, "Failed to load shader from filepath: \"{}\". Loaded files size {}", p_filepath, shaderCode.size());
                return false;
            }
            
            VkShaderModuleCreateInfo createInfo { };
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = shaderCode.size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

            const auto result = vkCreateShaderModule(_logicalLayer->GetLogicalDevice(),
                &createInfo, nullptr, p_shaderModule);

            if (result != VK_SUCCESS)
            {
                LOG_ERR(_logger, "Failed to create shader from filepath: \"{}\"", p_filepath);
                return false;
            }

            return true;
        }

    private:
        VkPipelineLayout _pipelineLayout;
        VkPipeline _pipeline;
        
        bool _initialized;

        const VulkanLogicalLayer* _logicalLayer;
        const VulkanSwapChain* _swapchain;
        const VulkanRenderPass* _renderPass;
        std::shared_ptr<Debug::Logger> _logger;
    };
    
}
