#pragma once
#include "Architecture/Internal/BusListener.h"
#include "BaseEvent.h"

namespace DeepEngine::Architecture
{

    using EventResult = Internal::BusPublishResult;

	class BaseEventListener : public Internal::BusListener<BaseEvent>
	{
		friend class EventBus;
        
	protected:
		BaseEventListener(std::function<void(BusListener*)> p_onDestroyCallback)
			: BusListener(std::move(p_onDestroyCallback))
		{ }
        
	public:
		BaseEventListener(const BaseEventListener&) = delete;
		BaseEventListener(BaseEventListener&&) = delete;
	};

	template <typename TEvent>
    requires std::is_base_of_v<Internal::BusObject, TEvent>
	class EventListener final : public BaseEventListener
	{
	public:
		EventListener(std::function<void(BusListener*)> p_onDestroyCallback)
			: BaseEventListener(std::move(p_onDestroyCallback))
		{
			_callbacks.reserve(16);
		}
        
		EventListener(const EventListener&) = delete;
		EventListener(EventListener&&) = delete;

		template <typename TPublisher>
		constexpr void BindCallback(EventResult (TPublisher::*p_func)(const TEvent&), TPublisher* p_publisher)
		{ BindCallback(std::bind(p_func, p_publisher, std::placeholders::_1)); }
		
		constexpr void BindCallback(std::function<EventResult(const TEvent&)> p_listenCallback)
		{
			_callbacks.push_back(p_listenCallback);
		}
		
		constexpr void UnbindAllCallbacks()
		{
			_callbacks.clear();
		}
		
		constexpr size_t GetListeningObjectType() const override
		{ return typeid(TEvent).hash_code(); }

	protected:
		EventResult PublishFromListeners(const BaseEvent* p_event) override
		{
			for (auto& callback : _callbacks)
			{
				EventResult result = callback(*(const TEvent*)p_event);
                
				if (result == EventResult::BLOCK)
				{
					return EventResult::BLOCK;
				}
			}

			return EventResult::PASS;
		}

	private:
		std::vector<std::function<EventResult(const TEvent&)>> _callbacks;
	};
	
}
