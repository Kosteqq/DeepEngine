#include "VulkanInstance.h"

#include "Architecture/EventBus/EventBus.h"

namespace DeepEngine::Renderer::Vulkan
{

	VulkanInstance::VulkanInstance(Architecture::EventBus& p_engineEventBus)
		: _engineEventBus(p_engineEventBus),
		_vulkanEventBus(p_engineEventBus.CreateChildEventBus())
	{
		_glfwWindowCreateListener = p_engineEventBus.CreateListener<Events::OnCreateGlfwContext>();
		_glfwWindowCreateListener->BindCallback<VulkanInstance>(&VulkanInstance::CreateGlfwWindowHandler, this);
		
		_windowFramebufferResizedListener = p_engineEventBus.CreateListener<Events::OnWindowFramebufferResized>();
		_windowFramebufferResizedListener->BindCallback<VulkanInstance>(&VulkanInstance::FramebufferResizedHandler, this);
		
		_enabledInstanceExtensionNames.reserve(32);
		_enabledPhysicalExtensionNames.reserve(32);
		_queuesCreateInfo.reserve(16);
		_queueInstances.reserve(16);

		_vulkanInstance = this;

		PreinitializeInstance();
	}

}
