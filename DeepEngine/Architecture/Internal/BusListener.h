#pragma once
#include <functional>

namespace DeepEngine::Architecture::Internal
{

	enum BusPublishResult
	{
		PASS = 0,
		BLOCK = 1,
	};

	template <typename TObject>
	class BusListener
	{
	protected:
		BusListener(std::function<void(BusListener*)>&& p_onDestroyCallback)
			: _onDestroyCallback(p_onDestroyCallback)
		{ }

	public:
		BusListener(const BusListener&) = delete;
		BusListener(BusListener&&) = delete;
    
		virtual ~BusListener()
		{
			_onDestroyCallback(this);
		}

	protected:
		virtual BusPublishResult PublishFromListeners(const TObject* p_event) = 0;
		virtual constexpr size_t GetListeningObjectType() const = 0;

	private:
		const std::function<void(BusListener*)> _onDestroyCallback;
	};
	
}
