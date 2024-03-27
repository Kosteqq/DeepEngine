#include "PipelineLayout.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{
	Ref<PipelineLayout> Factory::SubFactory<PipelineLayout>::Create(const Ref<RenderPass>& p_renderPass,
		RenderSubPassHandler p_subPass)
	{
		VkPipelineLayoutCreateInfo createInfo { };
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		createInfo.setLayoutCount = 0;
		createInfo.pSetLayouts = nullptr;
		createInfo.pushConstantRangeCount = 0;
		createInfo.pPushConstantRanges = nullptr;

		VkPipelineLayout handler;

		VULKAN_ASSERT_RESULT_V(
			vkCreatePipelineLayout(
				_bindFactory->_vulkanInstance.GetLogicalDevice(),
				&createInfo,
				nullptr,
				&handler),
			nullptr)

		auto pipelineLayout = new PipelineLayout(handler, p_subPass);
            
		return CreateObject(pipelineLayout, Terminate, p_renderPass);
	}

	void Factory::SubFactory<PipelineLayout>::Terminate(VulkanObject* p_object)
	{
		vkDestroyPipelineLayout(
			_bindFactory->_vulkanInstance.GetLogicalDevice(),
			((PipelineLayout*)p_object)->GetHandler(),
			nullptr);
	}
}
