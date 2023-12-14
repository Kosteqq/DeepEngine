#include "Events.h"

namespace DeepEngine::Architecture
{
    template <typename TEvent>
    requires std::is_base_of_v<Event, TEvent>
    static void PublishEvent(const TEvent& p_event)
    {
        const auto id = std::type_index(typeid(TEvent));

        if (!EventsInternal::BaseEventListener::_listenerInstances.contains(id))
        {
            return;
        }
            
        const auto& listeners = EventsInternal::BaseEventListener::_listenerInstances[id];

        for (uint32_t i = 0; i < listeners.size(); i++)
        {
            if (listeners[i]->InternalEventHandler((Event*)&p_event))
            {
                break;
            }
        }
    }
}
