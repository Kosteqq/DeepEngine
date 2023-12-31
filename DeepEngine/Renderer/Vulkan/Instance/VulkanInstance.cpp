#include "VulkanInstance.h"

#include "Architecture/EventBus/EventBus.h"

namespace DeepEngine::Renderer::Vulkan
{

	VulkanInstance::VulkanInstance(Architecture::EventBus& p_engineEventBus, Architecture::EventBus& p_rendererEventBus)
		: _engineEventBus(p_engineEventBus), _rendererEventBus(p_rendererEventBus),
		_vulkanEventBus(p_engineEventBus.CreateChildEventBus())
	{
		_glfwWindowCreateListener = p_engineEventBus.CreateListener<EngineEvents::OnCreateGlfwContext>();
		_glfwWindowCreateListener->BindCallback<VulkanInstance>(&VulkanInstance::CreateGlfwWindowHandler, this);
		
		_windowFramebufferResizedListener = p_engineEventBus.CreateListener<EngineEvents::OnWindowFramebufferResized>();
		_windowFramebufferResizedListener->BindCallback<VulkanInstance>(&VulkanInstance::FramebufferResizedHandler, this);
		
		_enabledInstanceExtensionNames.reserve(32);
		_enabledPhysicalExtensionNames.reserve(32);
		_queuesCreateInfo.reserve(16);
		_queueInstances.reserve(16);

		_vulkanInstance = this;

		PreinitializeInstance();
	}

}
