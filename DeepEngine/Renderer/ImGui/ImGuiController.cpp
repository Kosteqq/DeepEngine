#include "ImGuiController.h"

namespace DeepEngine::Renderer
{
	
	ImGuiController::ImGuiController(Vulkan::VulkanInstance* p_vulkanInstance,
		const Vulkan::VulkanInstance::QueueInstance* p_mainQueue, MainRenderPass* p_mainRenderPass): _vulkanInstance(p_vulkanInstance), _mainQueue(p_mainQueue), _mainRenderPass(p_mainRenderPass)
	{
		_imGuiRenderPass = new ImGuiRenderPass();
		if (!_vulkanInstance->InitializeSubController(_imGuiRenderPass))
		{
			return;
		}
			
		_commandPool = new Vulkan::CommandPool(_mainQueue, Vulkan::CommandPoolFlag::RESET_COMMAND_BUFFER);
		_vulkanInstance->InitializeSubController(_commandPool);

		_commandBuffers = _commandPool->CreateCommandBuffers(_vulkanInstance->GetSwapChainImageViews().size());
			
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();
			
		std::array pool_sizes
		{
			VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
		};
			
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 4;
		pool_info.poolSizeCount = pool_sizes.size();
		pool_info.pPoolSizes = pool_sizes.data();
		auto result = vkCreateDescriptorPool(
			_vulkanInstance->GetLogicalDevice(),
			&pool_info,
			nullptr,
			&_descPool);

		if (result != VK_SUCCESS)
		{
			return;
		}

		ImGui_ImplGlfw_InitForVulkan(_vulkanInstance->GetGlfwWindow(), true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = _vulkanInstance->GetVulkanInstance();
		init_info.PhysicalDevice = _vulkanInstance->GetPhysicalDevice();
		init_info.Device = _vulkanInstance->GetLogicalDevice();
		init_info.QueueFamily = _mainQueue->FamilyIndex;
		init_info.Queue = _mainQueue->Queue;
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = _descPool;
		init_info.Allocator = nullptr;
		init_info.MinImageCount = 2;
		init_info.ImageCount = static_cast<uint32_t>(_vulkanInstance->GetSwapChainImageViews().size());
		init_info.Subpass = 0;
		ImGui_ImplVulkan_Init(&init_info, _imGuiRenderPass->GetVkRenderPass());

		LoadFontLol();

		_attachmentsRecreatedListener = _vulkanInstance->GetRendererEventBus().CreateListener<MainRenderPassRecreatedAttachment>();
		_attachmentsRecreatedListener->BindCallback(&ImGuiController::RecreatedRenderPassAttachmentsHandler, this);
		CreateRenderPassTextures();
	}

	void ImGuiController::Terminate() const
	{
		for (VkDescriptorSet texture : _renderPassTextures)
		{
			ImGui_ImplVulkan_RemoveTexture(texture);
		}
		for (VkSampler sampler : _renderPassTexturesSamplers)
		{
			vkDestroySampler(_vulkanInstance->GetLogicalDevice(), sampler, nullptr);
		}
			
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		vkDestroyDescriptorPool(_vulkanInstance->GetLogicalDevice(), _descPool, nullptr);
	}

	void ImGuiController::Renderrr(uint32_t p_frameID)
	{
		// Start the Dear ImGui frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// ...
		ImGui::ShowDemoWindow();
		DrawVulkanStructureWindow();
		DrawViewportWindow(p_frameID);
			
		ImGui::Render();

		{
			auto result = vkResetCommandPool(_vulkanInstance->GetLogicalDevice(), _commandPool->GetVkCommandPool(), 0);

			if (result != VK_SUCCESS)
			{
				return;
			}
				
			VkCommandBufferBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
				
			result = vkBeginCommandBuffer(_commandBuffers[p_frameID]->GetVkCommandBuffer(), &info);

			if (result != VK_SUCCESS)
			{
				return;
			}
		}

		{
			VkClearValue clearColor { 0.1f, 0.1, 0.1f, 0.5f };
				
			VkRenderPassBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.renderPass = _imGuiRenderPass->GetVkRenderPass();
			info.framebuffer = _imGuiRenderPass->GetSwapchainImageVkFramebuffer(p_frameID);
			info.renderArea.extent.width = _vulkanInstance->GetFrameBufferSize().x;
			info.renderArea.extent.height = _vulkanInstance->GetFrameBufferSize().y;
			info.clearValueCount = 1;
			info.pClearValues = &clearColor;
			vkCmdBeginRenderPass(_commandBuffers[p_frameID]->GetVkCommandBuffer(), &info, VK_SUBPASS_CONTENTS_INLINE);
		}

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _commandBuffers[p_frameID]->GetVkCommandBuffer());

		vkCmdEndRenderPass(_commandBuffers[p_frameID]->GetVkCommandBuffer());
		auto result = vkEndCommandBuffer(_commandBuffers[p_frameID]->GetVkCommandBuffer());

		if (result != VK_SUCCESS)
		{
			return;
		}
	}

	void ImGuiController::PostRenderUpdate()
	{
		// Update and Render additional Platform Windows
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		if (_wasViewportResized)
		{
			_wasViewportResized = false;
			Events::OnViewportResized event;
			event.NewViewportSize = { _viewportSize.x, _viewportSize.y };
					
			_vulkanInstance->GetRendererEventBus().Publish(event);
		}
	}

	void ImGuiController::LoadFontLol()
	{
		// Load Fonts
		// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
		// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
		// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
		// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
		// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
		// - Read 'docs/FONTS.md' for more instructions and details.
		// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
		//io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
		//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
		//IM_ASSERT(font != nullptr);

		auto buffer = new Vulkan::CommandBuffer(_commandPool, false);
		_commandPool->InitializeSubController(buffer);

		// Upload Fonts
		{
			// Use any command queue
			VkResult err = vkResetCommandPool(_vulkanInstance->GetLogicalDevice(), _commandPool->GetVkCommandPool(), 0);
		    	
			if (err != VK_SUCCESS)
			{
				return;
			}
    			
			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			err = vkBeginCommandBuffer(buffer->GetVkCommandBuffer(), &begin_info);
		    	
			if (err != VK_SUCCESS)
			{
				return;
			}

			ImGui_ImplVulkan_CreateFontsTexture(buffer->GetVkCommandBuffer());

			VkSubmitInfo end_info = { };
			end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			end_info.commandBufferCount = 1;
			end_info.pCommandBuffers = &buffer->GetVkCommandBuffer();
			err = vkEndCommandBuffer(buffer->GetVkCommandBuffer());
		    	
			if (err != VK_SUCCESS)
			{
				return;
			}
		    	
			err = vkQueueSubmit(_mainQueue->Queue, 1, &end_info, VK_NULL_HANDLE);
		    	
			if (err != VK_SUCCESS)
			{
				return;
			}

			err = vkDeviceWaitIdle(_vulkanInstance->GetLogicalDevice());
		    	
			if (err != VK_SUCCESS)
			{
				return;
			}
		    	
			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}

		buffer->Terminate();
	}

	void ImGuiController::DrawViewportWindow(uint32_t p_frameID)
	{
		static bool isOpen = true;
			
		if (ImGui::Begin("Viewport", &isOpen))
		{
			auto size = glm::vec2 {
				ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x,
				ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y
			};

			if (_viewportSize.x != size.x || _viewportSize.y != size.y)
			{
				_wasViewportResized = true;
				_viewportSize = size;
			}

			ImGui::Image(_renderPassTextures[p_frameID], { _viewportSize.x, _viewportSize.y });
		}
			
		ImGui::End();
	}

	void ImGuiController::DrawVulkanStructureWindow()
	{
		static bool isOpen = true;
			
		if (ImGui::Begin("Vulkan Structure", &isOpen))
		{
			if (ImGui::TreeNode("Vulkan Instance"))
			{
				DrawVulkanControllerChilds(_vulkanInstance);
				ImGui::TreePop();
			}
		}

		ImGui::End();
	}

	void ImGuiController::DrawVulkanControllerChilds(Vulkan::BaseVulkanController* p_controller)
	{
		auto& childs = p_controller->GetChildControllers();
		auto it = childs.begin();
			
		for (uint32_t i = 0; i < childs.size(); i++)
		{
			ImGui::PushID(i);
			if (ImGui::TreeNode((*it)->GetDebugTypeName()))
			{
				DrawVulkanControllerChilds(*it);
				ImGui::TreePop();
			}
			ImGui::PopID();
				
			++it;
		}
	}

	Architecture::EventResult ImGuiController::RecreatedRenderPassAttachmentsHandler(
		const MainRenderPassRecreatedAttachment& p_event)
	{
		CreateRenderPassTextures();
		return Architecture::EventResult::PASS;
	}

	void ImGuiController::CreateRenderPassTextures()
	{
		for (VkDescriptorSet texture : _renderPassTextures)
		{
			ImGui_ImplVulkan_RemoveTexture(texture);
		}
		for (VkSampler sampler : _renderPassTexturesSamplers)
		{
			vkDestroySampler(_vulkanInstance->GetLogicalDevice(), sampler, nullptr);
		}

		_renderPassTexturesSamplers.clear();
		_renderPassTexturesSamplers.resize(_mainRenderPass->GetAllVkFramebuffer().size());
		_renderPassTextures.clear();
		_renderPassTextures.resize(_mainRenderPass->GetAllVkFramebuffer().size());

		for (uint32_t i = 0; i < static_cast<uint32_t>(_renderPassTextures.size()); i++)
		{
			VkSamplerCreateInfo samplerInfo { };
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = VK_FILTER_LINEAR;
			samplerInfo.minFilter = VK_FILTER_LINEAR;
			samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.anisotropyEnable = VK_FALSE;
			samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			samplerInfo.unnormalizedCoordinates = VK_FALSE;
			samplerInfo.compareEnable = VK_FALSE;
			samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.mipLodBias = 0.0f;
			samplerInfo.minLod = 0.0f;
			samplerInfo.maxLod = 0.0f;

			vkCreateSampler(_vulkanInstance->GetLogicalDevice(), &samplerInfo, nullptr, &_renderPassTexturesSamplers[i]);
				
			_renderPassTextures[i] = ImGui_ImplVulkan_AddTexture(_renderPassTexturesSamplers[i], _mainRenderPass->GetVkImageView(i), VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL);
		}
	}
	
}
