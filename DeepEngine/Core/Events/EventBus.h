#pragma once
#include <stack>

#include "Debug/Logger.h"
#include "Core/Bus/BusObject.h"
#include "Core/Bus/Bus.h"

#include "BaseEvent.h"
#include "EventListener.h"


namespace DeepEngine::Core::Events
{

    class EventBus : public Bus::Bus<BaseEvent, BaseEventListener>
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
        requires std::is_base_of_v<Core::Bus::BusObject, TEvent>
        constexpr std::shared_ptr<EventListener<TEvent>> CreateListener()
        { return AddListener<EventListener<TEvent>>(); }

        template <typename T>
        requires std::is_base_of_v<BaseEvent, T>
        void Publish(const T& p_event)
        { OnPublish(p_event); }

        template <typename T>
        requires std::is_base_of_v<BaseEvent, T>
        void Publish()
        { OnPublish(T()); }

        constexpr EventBus& CreateChildEventBus()
        { return static_cast<EventBus&>(CreateChildBus()); }

    private:
        void OnPublish(const BaseEvent& p_event);
        
        static EventResult PublishInListeners(const EventBus* p_bus, const BaseEvent& p_event);
        static EventBus* GetTopParent(EventBus* p_bus);
    };
    
}
