#pragma once
#include <list>
#include <memory>

namespace DeepEngine::Engine::Renderer::Vulkan
{

	class VulkanObject
	{
		friend class VulkanFactory;
		typedef void(TerminateFunction)(VulkanObject*);

	public:
		virtual ~VulkanObject() = default;

		bool IsValid() const
		{
			return _isValid;
		}
    
	private:
		bool _isValid;
		TerminateFunction* _terminateFunc;
		
		std::list<std::weak_ptr<VulkanObject>> _subobjects;
	};
	
}
