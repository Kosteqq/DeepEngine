#include "Fence.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{
	Ref<Fence> Factory::SubFactory<Fence>::Create(const Ref<VulkanObject>& p_parent)
	{
		VkFenceCreateInfo fenceCreateInfo { };
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            
		VkFence handler;
		VULKAN_ASSERT_RESULT_V(
			vkCreateFence(_bindFactory->_vulkanInstance.GetLogicalDevice(), &fenceCreateInfo, nullptr, &handler),
			nullptr)

		return CreateObject(new Fence(handler), Terminate, p_parent);
	}

	Ref<Fence> Factory::SubFactory<Fence>::CreateSignaled(const Ref<VulkanObject>& p_parent)
	{
		VkFenceCreateInfo fenceCreateInfo { };
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
            
		VkFence handler;

		VULKAN_ASSERT_RESULT_V(
			vkCreateFence(_bindFactory->_vulkanInstance.GetLogicalDevice(), &fenceCreateInfo, nullptr, &handler),
			nullptr)

		return CreateObject(new Fence(handler), Terminate, p_parent);
	}

	void Factory::SubFactory<Fence>::Terminate(VulkanObject* p_object)
	{
		auto fenceObject = (Fence*)p_object;
		vkDestroyFence(_bindFactory->_vulkanInstance.GetLogicalDevice(), fenceObject->GetHandler(), nullptr);
	}
}
