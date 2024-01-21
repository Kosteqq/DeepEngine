#pragma once

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

#include "ImGuiRenderPass.h"
#include "Core/Scene/Scene.h"
#include "Engine/Renderer/Vulkan/CommandBuffer.h"
#include "Engine/Renderer/Vulkan/CommandPool.h"
#include "Engine/Renderer/Vulkan/Instance/VulkanInstance.h"
#include "Engine/Renderer/Vulkan/Events/VulkanEvents.h"
#include "Engine/Renderer/Events.h"
#include "Engine/Renderer/MainRenderPass.h"

namespace DeepEngine::Engine::Renderer
{

	class ImGuiController
	{
		using OnSwapChainRecreated = Vulkan::Events::OnSwapChainRecreated;

	public:
		ImGuiController(Vulkan::VulkanInstance* p_vulkanInstance, const Vulkan::VulkanInstance::QueueInstance* p_mainQueue,
			MainRenderPass* p_mainRenderPass);

		void Terminate() const;

		void Renderrr(uint32_t p_frameID, const Core::Scene::Scene& p_scene);
		void PostRenderUpdate();

		Vulkan::CommandBuffer* GetCommandBuffer(uint32_t p_frameID) const
		{
			return _commandBuffers[p_frameID];
		}

	private:
		void LoadFontLol();

		void DrawViewportWindow(uint32_t p_frameID);
		void DrawVulkanStructureWindow();
		void DrawVulkanControllerChilds(Vulkan::BaseVulkanController* p_controller);
		void DrawScene(const Core::Scene::Scene& p_scene);

		Core::Events::EventResult RecreatedRenderPassAttachmentsHandler(const MainRenderPassRecreatedAttachment& p_event);

		void CreateRenderPassTextures();

	private:
		// From where it should be take from????
		const uint32_t _minImageCount = 2;
		VkDescriptorPool _descPool;

		glm::vec2 _viewportSize;
		bool _wasViewportResized;

		Vulkan::VulkanInstance* _vulkanInstance;
		const Vulkan::VulkanInstance::QueueInstance* _mainQueue;
		Vulkan::CommandPool* _commandPool;
		std::vector<Vulkan::CommandBuffer*> _commandBuffers;
        ImGuiRenderPass* _imGuiRenderPass = nullptr;
        MainRenderPass* _mainRenderPass = nullptr;

        std::vector<VkSampler> _renderPassTexturesSamplers;
        std::vector<VkDescriptorSet> _renderPassTextures;

		std::shared_ptr<Core::Events::EventListener<MainRenderPassRecreatedAttachment>> _attachmentsRecreatedListener;
	};

}
