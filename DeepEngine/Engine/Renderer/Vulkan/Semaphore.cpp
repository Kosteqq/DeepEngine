#include "Semaphore.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{
	Ref<Semaphore> Factory::SubFactory<Semaphore>::Create(const Ref<VulkanObject>& p_parent)
	{
		VkSemaphoreCreateInfo createInfo { };
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		createInfo.flags = 0;
        
		VkSemaphore handler;
		vkCreateSemaphore(
			_bindFactory->_vulkanInstance.GetLogicalDevice(),
			&createInfo,
			nullptr,
			&handler);
        
		return CreateObject(new Semaphore(handler), Terminate, p_parent);
	}

	void Factory::SubFactory<Semaphore>::Terminate(VulkanObject* p_object)
	{
		vkDestroySemaphore(
			_bindFactory->_vulkanInstance.GetLogicalDevice(),
			((Semaphore*)p_object)->GetHandler(),
			nullptr);
	}
}
