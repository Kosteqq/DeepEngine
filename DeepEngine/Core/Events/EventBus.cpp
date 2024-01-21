#include "EventBus.h"

#include "Debug/Timing.h"

namespace DeepEngine::Core::Events
{
    void EventBus::OnPublish(const BaseEvent& p_event)
    {
    	struct PublishStack
    	{
    		EventBus* Bus;
    		uint32_t BusChildIndex;
    	};
            
    	TIMER(fmt::format("Publishing \"{}\" event", p_event.GetName()).c_str());
    	EventBus* rootBus;

    	switch (p_event.GetPublishingScope())
    	{
    	case EventScope::LOCAL:
    		rootBus = this;
    		break;
    	case EventScope::GLOBAL:
    		rootBus = GetTopParent(this);
    		break;
    	default:
    		ENGINE_ERR("Unhandled EventScope");
    		assert(false);
    		return;
    	}

    	std::stack<PublishStack> stack;
    	stack.emplace(rootBus, 0);
            
    	while (stack.size() > 0)
    	{
    		PublishStack& stackElement = stack.top();
    		const auto& childs = stackElement.Bus->GetChildBuses();

    		// Publish in self listeners before move to first child bus
    		if (stackElement.BusChildIndex == 0)
    		{
    			if (PublishInListeners(stackElement.Bus, p_event) == EventResult::BLOCK)
    			{
    				break;
    			}
    		}

    		if (stackElement.BusChildIndex >= childs.size())
    		{
    			stack.pop();
    			continue;
    		}

    		EventBus* nextChild = (EventBus*)(&childs[stackElement.BusChildIndex]);
    		stackElement.BusChildIndex++;
                
    		stack.emplace(nextChild, 0);
    	}
    }
	
	EventResult EventBus::PublishInListeners(const EventBus* p_bus, const BaseEvent& p_event)
	{
		for (BaseEventListener* listener : p_bus->GetListeners())
		{
			if (listener->GetListeningObjectType() != p_event.GetTypeID())
			{
				continue;
			}

			if (listener->PublishFromListeners(&p_event) == EventResult::BLOCK)
			{
				return EventResult::BLOCK;
			}
		}
            
		return EventResult::PASS;
	}

	EventBus* EventBus::GetTopParent(EventBus* p_bus)
	{
		auto parentBus = (EventBus*)p_bus->GetParentBus();
		if (parentBus != nullptr)
		{
			return GetTopParent(parentBus);
		}

		return p_bus;
	}
}
