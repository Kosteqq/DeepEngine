#include "VulkanFactory.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{
	VulkanFactory* VulkanFactory::_bindFactory = nullptr;

	VulkanFactory::VulkanFactory(VulkanInstance& p_vulkanInstance): _vulkanInstance(p_vulkanInstance)
	{
        
	}

	VulkanFactory::~VulkanFactory()
	{
		for (auto it = _parentlessObjects.rbegin(); it != _parentlessObjects.rend(); ++it)
		{
			TerminateObject(*it);
		}
	}

	void VulkanFactory::Bind()
	{
		_bindFactory = this;
	}

	void VulkanFactory::TerminateObject(const std::shared_ptr<VulkanObject>& p_object)
	{
		_bindFactory->_parentlessObjects.remove(p_object.get());

		TerminateObject(p_object.get());
	}

	void VulkanFactory::TerminateObject(VulkanObject* p_object)
	{
		if (!p_object->IsValid())
		{
			return;
		}
		
		for (auto it = p_object->_subobjects.rbegin(); it != p_object->_subobjects.rend(); ++it)
		{
			if (!it->expired())
			{
				TerminateObject(it->lock().get());
			}
		}
        
		p_object->_terminateFunc(p_object);

		p_object->_isValid = false;
	}

	void VulkanFactory::DestroyPointerHandler(VulkanObject* p_object)
	{
		TerminateObject(p_object);
		delete p_object;
	}
}
