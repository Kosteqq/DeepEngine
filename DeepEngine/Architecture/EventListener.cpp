#include "Events.h"

namespace DeepEngine::Architecture::EventsInternal
{
    std::unordered_map<std::type_index, std::vector<BaseEventListener*>> BaseEventListener::_listenerInstances;
    
    BaseEventListener::BaseEventListener(const std::type_index p_eventType): m_EventType(p_eventType)
    {
        if (!_listenerInstances.contains(p_eventType))
        {
            _listenerInstances[p_eventType] = std::vector<BaseEventListener*>();
            _listenerInstances[p_eventType].reserve(16);
        }
            
        _listenerInstances[p_eventType].push_back(this);
    }

    BaseEventListener::~BaseEventListener()
    {
        if (!_listenerInstances.contains(m_EventType))
        {
            return;
        }
            
        auto& listeners = _listenerInstances[m_EventType];
        for (uint32_t i = 0; i < listeners.size(); i++)
        {
            if (listeners[i] == this)
            {
                listeners[i] = nullptr;
            }
        }
    }
}
