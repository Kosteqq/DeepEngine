#pragma once
#include <stack>

#include "Debugs/Logger.h"
#include "Architecture/Internal/Bus.h"

#include "BaseEvent.h"
#include "EventListener.h"


namespace DeepEngine::Architecture
{

    class EventBus : public Internal::Bus<BaseEvent, BaseEventListener>
    {
    private:
        EventBus(EventBus* p_parent) : Bus(p_parent)
        { }
        
    public:
        EventBus() : Bus()
        { }
        EventBus(const EventBus&) = delete;
        EventBus(EventBus&&) = delete;

        template <typename TEvent>
        requires std::is_base_of_v<Internal::BusObject, TEvent>
        std::shared_ptr<EventListener<TEvent>> CreateListener()
        { return AddListener<EventListener<TEvent>>(); }

        template <typename T>
        requires std::is_base_of_v<BaseEvent, T>
        void Publish(const T& p_event)
        { OnPublish(p_event); }

    private:
        void OnPublish(const BaseEvent& p_event);
        
        static EventResult PublishInListeners(const EventBus* p_bus, const BaseEvent& p_event);
        static EventBus* GetTopParent(EventBus* p_bus);
    };
    
}
