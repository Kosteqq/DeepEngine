#include "CommandPool.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{
	Ref<CommandPool> Factory::SubFactory<CommandPool>::Create(const VulkanInstance::QueueInstance* p_queue,
		CommandPoolFlag p_flags, Ref<VulkanObject> p_parent)
	{
		VkCommandPoolCreateInfo createInfo { };
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.flags = 0;
		createInfo.queueFamilyIndex = p_queue->FamilyIndex;

		if (p_flags & PROTECTED)
		{
			createInfo.flags |= VK_COMMAND_POOL_CREATE_PROTECTED_BIT;
		}
		if (p_flags & TRANSIENT)
		{
			createInfo.flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		}
		if (p_flags & RESET_COMMAND_BUFFER)
		{
			createInfo.flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		}

		VkCommandPool commandPool;
		vkCreateCommandPool(
			_bindFactory->_vulkanInstance.GetLogicalDevice(),
			&createInfo,
			nullptr,
			&commandPool);

		auto poolObject = new CommandPool(commandPool, p_queue);
            
		return CreateObject(poolObject, Terminate, p_parent);
	}

	void Factory::SubFactory<CommandPool>::Terminate(VulkanObject* p_object)
	{
		auto poolObject = (CommandPool*)p_object;
            
		vkDestroyCommandPool(
			_bindFactory->_vulkanInstance.GetLogicalDevice(),
			poolObject->GetHandler(),
			nullptr);

	}
}
