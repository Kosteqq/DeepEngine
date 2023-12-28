#pragma once

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

#include "Renderer/ImGuiRenderPass.h"
#include "Renderer/Vulkan/CommandBuffer.h"
#include "Renderer/Vulkan/CommandPool.h"
#include "Renderer/Vulkan/Instance/VulkanInstance.h"
#include "Renderer/Vulkan/Events/VulkanEvents.h"

namespace DeepEngine::Renderer
{

	class ImGuiController
	{
		using OnSwapChainRecreated = Vulkan::Events::OnSwapChainRecreated;
		
	public:
		ImGuiController(Vulkan::VulkanInstance* p_vulkanInstance, const Vulkan::VulkanInstance::QueueInstance* p_mainQueue,
			const ImGuiRenderPass* p_imGuiRenderPass)
			: _vulkanInstance(p_vulkanInstance), _mainQueue(p_mainQueue), _imGuiRenderPass(p_imGuiRenderPass)
		{
			_commandPool = new Vulkan::CommandPool(_mainQueue, Vulkan::CommandPoolFlag::RESET_COMMAND_BUFFER);
			_vulkanInstance->InitializeSubController(_commandPool);

			// _commandBuffers.resize(_vulkanInstance->GetSwapChainImageViews().size());
			// for (uint32_t i = 0; i < _vulkanInstance->GetSwapChainImageViews().size(); i++)
			// {
			// 	_commandBuffers[i] = new Vulkan::CommandBuffer(_commandPool, false);
			// 	_commandPool->InitializeSubController(_commandBuffers[i]);
			// }
			_commandBuffers = _commandPool->CreateCommandBuffers(_vulkanInstance->GetSwapChainImageViews().size());
			
			_swapChainRecreatedListener = p_vulkanInstance->GetVulkanEventBus().CreateListener<OnSwapChainRecreated>();
			_swapChainRecreatedListener->BindCallback(&ImGuiController::SwapChainRecreatedHandler, this);
			
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
			
			std::array<VkDescriptorPoolSize, 1> pool_sizes
			{
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1 },
			};
			
			VkDescriptorPoolCreateInfo pool_info = {};
			pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			pool_info.maxSets = 1;
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
		}

		void Terminate()
		{
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
			vkDestroyDescriptorPool(_vulkanInstance->GetLogicalDevice(), _descPool, nullptr);
		}

		void Renderrr(uint32_t p_frameID)
		{
			// Start the Dear ImGui frame
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// ...
			ImGui::ShowDemoWindow();
			
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

			// Update and Render additional Platform Windows
			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
			}
		}

		Vulkan::CommandBuffer* GetCommandBuffer(uint32_t p_frameID) const
		{
			return _commandBuffers[p_frameID];
		}
		
	private:
		

		Architecture::EventResult SwapChainRecreatedHandler(const OnSwapChainRecreated&)
		{
			// ImGui_ImplVulkan_SetMinImageCount(_minImageCount);
			// ImGui_ImplVulkanH_CreateOrResizeWindow(
			// 	_vulkanInstance->GetVulkanInstance(),
			// 	_vulkanInstance->GetPhysicalDevice(),
			// 	_vulkanInstance->GetLogicalDevice(),
			// 	&_window,
			// 	_mainQueue->FamilyIndex,
			// 	nullptr,
			// 	_vulkanInstance->GetFrameBufferSize().x,
			// 	_vulkanInstance->GetFrameBufferSize().y,
			// 	_minImageCount);
			//
			// _window.FrameIndex = 0;

			return Architecture::EventResult::PASS;
		}

		void LoadFontLol()
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

			delete buffer;
		}
		
	private:

		// From where it should be take from????
		const uint32_t _minImageCount = 2;
		VkDescriptorPool _descPool;

		Vulkan::VulkanInstance* _vulkanInstance;
		const Vulkan::VulkanInstance::QueueInstance* _mainQueue;
		Vulkan::CommandPool* _commandPool;
		std::vector<Vulkan::CommandBuffer*> _commandBuffers;
		const ImGuiRenderPass* _imGuiRenderPass;

		std::shared_ptr<Architecture::EventListener<OnSwapChainRecreated>> _swapChainRecreatedListener;
	};
	
}
