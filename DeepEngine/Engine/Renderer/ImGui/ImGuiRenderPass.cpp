#include "ImGuiRenderPass.h"

namespace DeepEngine::Engine::Renderer
{

	void ImGuiRenderPass::Initialize()
	{
		auto vulkanController = GetVulkanInstanceController();
            
		VkAttachmentDescription baseColorAttachmentDesc { };
		baseColorAttachmentDesc.format = vulkanController->GetSwapchainCurrentFormat().format;
		baseColorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		baseColorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		baseColorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		baseColorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		baseColorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		baseColorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		baseColorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkSubpassDependency dependency { };
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            
		CreateRenderAttachment(baseColorAttachmentDesc, &_colorAttachment);
            
		CreateRenderSubPass(VK_PIPELINE_BIND_POINT_GRAPHICS)
			.AddColorAttachment(_colorAttachment, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
			.AddDependency(dependency)
			.GetSubPassPtr(&_baseSubPass);

		_swapChainRecreatedListener = vulkanController->GetVulkanEventBus().CreateListener<Vulkan::Events::OnSwapChainRecreated>();
		_swapChainRecreatedListener->BindCallback(&ImGuiRenderPass::SwapChainRecreatedHandler, this);
	}

	void ImGuiRenderPass::PostInitialize()
	{
		RecreateFrameBuffers();
	}

	void ImGuiRenderPass::OnTerminate()
	{
		RenderPass::OnTerminate();
            
		for (const VkFramebuffer framebuffer : _swapchainImageFramebuffers)
		{
			vkDestroyFramebuffer(GetVulkanInstanceController()->GetLogicalDevice(), framebuffer, nullptr);
		}
	}

	Vulkan::PipelineLayout* ImGuiRenderPass::CreateBaseSubPassPipelineLayout()
	{
		auto pipelineLayout = new Vulkan::PipelineLayout(this, _baseSubPass->ID);
		if (!InitializeSubController(pipelineLayout))
		{
			pipelineLayout->Terminate();
			return nullptr;
		}
            
		return pipelineLayout;
	}

	VkFramebuffer ImGuiRenderPass::GetSwapchainImageVkFramebuffer(uint32_t p_index) const
	{
		return _swapchainImageFramebuffers[p_index];
	}

	void ImGuiRenderPass::RecreateFrameBuffers()
	{
		const std::vector<VkImageView>& imageViews = GetVulkanInstanceController()->GetSwapChainImageViews();
		glm::vec2 framebufferSize = GetVulkanInstanceController()->GetFrameBufferSize();

		_swapchainImageFramebuffers.clear();
		_swapchainImageFramebuffers.reserve(imageViews.size());
            
		for (uint32_t i = 0; i < imageViews.size(); i++)
		{
			VkImageView attachments[] { imageViews[i] };
                    
			VkFramebufferCreateInfo createInfo { };
			createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			createInfo.renderPass = GetVkRenderPass();
			createInfo.attachmentCount = 1;
			createInfo.pAttachments = attachments;
			createInfo.width = framebufferSize.x;
			createInfo.height = framebufferSize.y;
			createInfo.layers = 1;

			VkFramebuffer _framebuffer;
			VkResult result = vkCreateFramebuffer(
				GetVulkanInstanceController()->GetLogicalDevice(),
				&createInfo,
				nullptr,
				&_framebuffer);

			if (result != VK_SUCCESS)
			{
				VULKAN_ERR("Failed to create SwapChain image view! (result: {})", string_VkResult(result));
				continue;
			}

			_swapchainImageFramebuffers.push_back(_framebuffer);
		}
	}

	Core::Events::EventResult ImGuiRenderPass::SwapChainRecreatedHandler(const Vulkan::Events::OnSwapChainRecreated& p_event)
	{
		RecreateFrameBuffers();
		return Core::Events::EventResult::PASS;
	}
	
}
