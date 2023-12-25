#include "Bus.h"

namespace DeepEngine::Architecture::Internal
{

	template <typename TObject, typename TListener>
	requires std::is_base_of_v<BusObject, TObject> && std::is_base_of_v<BusListener<TObject>, TListener>
	Bus<TObject, TListener>::Bus(Bus<TObject, TListener>* p_parent): _parentBus(p_parent)
	{
		_listeners.reserve(32);
		_childBuses.reserve(32);
	}

	template <typename TObject, typename TListener>
	requires std::is_base_of_v<BusObject, TObject> && std::is_base_of_v<BusListener<TObject>, TListener>
	void Bus<TObject, TListener>::DestroyListenerHandler(BusListener<TObject>* p_destroyedListener)
	{
		for (uint32_t i = 0; i < _listeners.size(); i++)
		{
			if (_listeners[i] == p_destroyedListener)
			{
				_listeners.erase(_listeners.begin() + i);
				break;
			}
		}
	}
	
}