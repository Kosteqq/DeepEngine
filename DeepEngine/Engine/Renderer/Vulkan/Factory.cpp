#include "Factory.h"

namespace DeepEngine::Engine::Renderer::Vulkan
{
	Factory* Factory::_bindFactory = nullptr;

	Factory::Factory(VulkanInstance& p_vulkanInstance): _vulkanInstance(p_vulkanInstance)
	{
        
	}

	Factory::~Factory()
	{
		while (!_parentlessObjects.empty())
		{
			TerminateObject(_parentlessObjects.back());
		}
		
		if (_bindFactory == this)
		{
			_bindFactory = nullptr;
		}
	}

	void Factory::Bind()
	{
		_bindFactory = this;
	}

	void Factory::TerminateObject(const std::shared_ptr<VulkanObject>& p_object)
	{
		TerminateObject(p_object.get());
	}

	void Factory::TerminateObject(VulkanObject* p_object)
	{
		if (!p_object->IsValid()
			|| _bindFactory == nullptr)
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

		_bindFactory->_parentlessObjects.remove(p_object);
		
		p_object->_terminateFunc(p_object);

		p_object->_isValid = false;
	}

	void Factory::DestroyPointerHandler(VulkanObject* p_object)
	{
		TerminateObject(p_object);
		delete p_object;
	}
}
