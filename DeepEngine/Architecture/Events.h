#pragma once
#include <functional>
#include <typeindex>

#define DECLARE_EVENT(EventName) struct EventName : private DeepEngine::Architecture::Event

namespace DeepEngine::Architecture
{
    struct Event
    { };

    template <typename TEvent>
    requires std::is_base_of_v<Event, TEvent>
    static void PublishEvent(const TEvent& p_event);
    
    
    namespace EventsInternal
    {
        class BaseEventListener
        {
            template <typename TEvent>
            requires std::is_base_of_v<Event, TEvent>
            friend static void DeepEngine::Architecture::PublishEvent(const TEvent& p_event);
        
        protected:
            BaseEventListener(const std::type_index p_eventType);
            virtual ~BaseEventListener();

            bool virtual InternalEventHandler(const Event* p_event) = 0;
        
        private:
            const std::type_index m_EventType;
            static std::unordered_map<std::type_index, std::vector<BaseEventListener*>> _listenerInstances;
        };
    }

    template <typename TEvent>
    requires std::is_base_of_v<Event, TEvent>
    class EventListener : protected EventsInternal::BaseEventListener
    {
    protected:
        EventListener()
            : EventsInternal::BaseEventListener(std::type_index(typeid(TEvent)))
        { }

        bool InternalEventHandler(const Event* p_event) override
        { return EventHandler((const TEvent*)(p_event)); }
        
        bool virtual EventHandler(const TEvent* p_event) = 0;
    };
}

#include "Events.tpp"
