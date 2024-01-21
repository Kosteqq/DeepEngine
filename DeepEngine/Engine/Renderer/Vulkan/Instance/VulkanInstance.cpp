#include "VulkanInstance.h"

#include "Core/Events/EventBus.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{

	VulkanInstance::VulkanInstance(Core::Events::EventBus& p_engineEventBus, Core::Events::EventBus& p_rendererEventBus)
		: _engineEventBus(p_engineEventBus), _rendererEventBus(p_rendererEventBus),
		_vulkanEventBus(p_engineEventBus.CreateChildEventBus())
	{
		_glfwWindowCreateListener = p_engineEventBus.CreateListener<Core::Events::OnCreateGlfwContext>();
		_glfwWindowCreateListener->BindCallback<VulkanInstance>(&VulkanInstance::CreateGlfwWindowHandler, this);
		
		_windowFramebufferResizedListener = p_engineEventBus.CreateListener<Core::Events::OnWindowFramebufferResized>();
		_windowFramebufferResizedListener->BindCallback<VulkanInstance>(&VulkanInstance::FramebufferResizedHandler, this);
		
		_enabledInstanceExtensionNames.reserve(32);
		_enabledPhysicalExtensionNames.reserve(32);
		_queuesCreateInfo.reserve(16);
		_queueInstances.reserve(16);

		_vulkanInstance = this;

		PreinitializeInstance();
	}

}
