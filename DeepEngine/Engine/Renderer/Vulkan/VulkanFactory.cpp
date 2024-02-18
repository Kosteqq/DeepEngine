#include "VulkanFactory.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{
	VulkanFactory* VulkanFactory::_bindInstance = nullptr;

	VulkanFactory::VulkanFactory(VulkanInstance& p_vulkanInstance): _vulkanInstance(p_vulkanInstance)
	{
        
	}

	void VulkanFactory::Bind()
	{
		_bindInstance = this;
	}

	void VulkanFactory::TerminateObject(const std::shared_ptr<VulkanObject>& p_object)
	{
		TerminateObject(p_object.get());
	}

	void VulkanFactory::TerminateObject(VulkanObject* p_object)
	{
		if (!p_object->IsValid())
		{
			return;
		}
		std::cout << "Terminating Object" << std::endl;

		// revers it lol
		for (auto& subObject : p_object->_subobjects)
		{
			if (!subObject.expired())
			{
				TerminateObject(subObject.lock().get());
			}
		}
        
		p_object->_terminateFunc(p_object);

		p_object->_isValid = false;
	}

	void VulkanFactory::DestroyPointerHandler(VulkanObject* p_object)
	{
		std::cout << "Destroying Pointer" << std::endl;
		TerminateObject(p_object);
		delete p_object;
	}

	VulkanFactory* VulkanFactory::GetInstance()
	{ return VulkanFactory::_bindInstance; }
}
