#include "GraphicsPipeline.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{
	Ref<GraphicsPipeline> Factory::SubFactory<GraphicsPipeline>::Create(const Ref<RenderPass>& p_renderPass,
		const Ref<PipelineLayout>& p_pipelineLayout, GraphicsPipelineBuilder* p_builder)
	{
		std::vector<VkPipelineShaderStageCreateInfo> stages;

		Ref<ShaderModule> vertShader;
		Ref<ShaderModule> fragShader;

		p_builder->GetShaderStages(vertShader, fragShader);
		InitializeShaderStages(vertShader, fragShader, &stages);

		GraphicsPipelineBuilder::DynamicStateDefinition dynamicStatesDefinition;
		p_builder->DefineDynamicState(dynamicStatesDefinition);

		std::vector<VkDynamicState> dynamicStates;
		InitializeDynamicStates(dynamicStatesDefinition, dynamicStates);
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo { };
		dynamicStateCreateInfo.sType              = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.dynamicStateCount  = static_cast<uint32_t>(dynamicStates.size());
		dynamicStateCreateInfo.pDynamicStates     = dynamicStates.data();

		VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo { };
		InitializeVertexInputStage(&vertexInputCreateInfo);

		GraphicsPipelineBuilder::RasterisationDefinition rasterisaztionDefinition { };
		p_builder->DefineRasterisation(rasterisaztionDefinition);
            
		VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo { };

		if (!InitializeRasterization(rasterisaztionDefinition, &rasterizerCreateInfo))
		{
			VULKAN_ERR("Failed to initialize graphics pipeline rasterisation");
			return nullptr;
		}

		// Get info about attachments
		std::vector<GraphicsPipelineBuilder::ColorBlendAttachmentDefinition> attachmentBlendDefinitions(p_renderPass->GetAttachments().size());
		for (uint64_t i = 0; i < attachmentBlendDefinitions.size(); i++)
		{
			p_builder->DefineBlendAttachment(i, attachmentBlendDefinitions[i]);
		}

		GraphicsPipelineBuilder::ColorBlendDefinition colorBlendDefinition;
		p_builder->DefineColorBlend(colorBlendDefinition);

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

		return CreateObject(new GraphicsPipeline(pipelineHandler), Terminate, p_pipelineLayout);
	}

	void Factory::SubFactory<GraphicsPipeline>::InitializeShaderStages(const Ref<ShaderModule>& p_vertShader,
		const Ref<ShaderModule>& p_fragShader, std::vector<VkPipelineShaderStageCreateInfo>* p_stages)
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

	void Factory::SubFactory<GraphicsPipeline>::InitializeDynamicStates(
		const GraphicsPipelineBuilder::DynamicStateDefinition& p_stateDefinition,
		std::vector<VkDynamicState>& p_dynamicStates)
	{
		p_dynamicStates.reserve(16);
                
		if (p_stateDefinition.Viewport)
		{
			p_dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
		}
                
		if (p_stateDefinition.Scissor)
		{
			p_dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
		}
                
		if (p_stateDefinition.LineWidth)
		{
			p_dynamicStates.push_back(VK_DYNAMIC_STATE_LINE_WIDTH);
		}
	}

	void Factory::SubFactory<GraphicsPipeline>::InitializeVertexInputStage(
		VkPipelineVertexInputStateCreateInfo* p_vertexInputCreateInfo)
	{
		p_vertexInputCreateInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		p_vertexInputCreateInfo->vertexBindingDescriptionCount    = 0;
		p_vertexInputCreateInfo->pVertexBindingDescriptions       = nullptr;
		p_vertexInputCreateInfo->vertexAttributeDescriptionCount  = 0;
		p_vertexInputCreateInfo->pVertexAttributeDescriptions     = nullptr;
	}

	bool Factory::SubFactory<GraphicsPipeline>::InitializeRasterization(
		const GraphicsPipelineBuilder::RasterisationDefinition& p_rasterisationDefinition,
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

	void Factory::SubFactory<GraphicsPipeline>::InitializeColorBlend(
		const std::vector<GraphicsPipelineBuilder::ColorBlendAttachmentDefinition>& p_attachemntsBlendDefinition,
		const GraphicsPipelineBuilder::ColorBlendDefinition& p_colorBlendDefinition,
		std::vector<VkPipelineColorBlendAttachmentState>* p_attachmentsBlends,
		VkPipelineColorBlendStateCreateInfo* p_colorBlendCreateInfo)
	{
		for (uint64_t i = 0; i < p_attachmentsBlends->size(); i++)
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

	void Factory::SubFactory<GraphicsPipeline>::Terminate(VulkanObject* p_object)
	{
		vkDestroyPipeline(
			_bindFactory->_vulkanInstance.GetLogicalDevice(),
			((GraphicsPipeline*)p_object)->GetHandler(),
			nullptr);
	}
}
