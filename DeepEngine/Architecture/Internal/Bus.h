#pragma once
#include <functional>
#include <memory>

#include "BusObject.h"
#include "BusListener.h"

namespace DeepEngine::Architecture::Internal
{

	template <typename TObject, typename TListener>
	requires std::is_base_of_v<BusObject, TObject>
		&& std::is_base_of_v<BusListener<TObject>, TListener>
	class Bus
	{
	protected:
		Bus() = default;
		Bus(Bus<TObject, TListener>* p_parent);

	public:
		Bus(const Bus&) = delete;
		Bus(Bus&&) = delete;
    
		virtual ~Bus() = default;

	protected:
		template <typename T = TListener, typename ...TArgs>
		requires std::is_base_of_v<TListener, T>
		constexpr std::shared_ptr<T> AddListener(TArgs... p_args)
		{
			auto ptr = std::make_shared<T>(p_args, [&](BusListener<TObject>* p_listener) { DestroyListenerHandler(p_listener); }) ;
			_listeners.push_back(ptr.get());
			return ptr;
		}
    
		template <typename T = TListener>
		requires std::is_base_of_v<TListener, T>
		constexpr std::shared_ptr<T> AddListener()
		{
			auto ptr = std::make_shared<T>([&](BusListener<TObject>* p_listener) { DestroyListenerHandler(p_listener); }) ;
			_listeners.push_back(ptr.get());
			return ptr;
		}

		constexpr Bus& CreateChildBus()
		{
			_childBuses.emplace_back(this);
			return _childBuses.back();
		}
    
		constexpr const std::vector<TListener*>& GetListeners() const
		{ return _listeners; }
		constexpr Bus<TObject, TListener>* GetParentBus() const
		{ return _parentBus; }
		constexpr const std::vector<Bus<TObject, TListener>*>& GetChildBuses() const
		{ return _childBuses; }

	private:
		void DestroyListenerHandler(BusListener<TObject>* p_destroyedListener);

	private:
		std::vector<TListener*> _listeners;
		Bus<TObject, TListener>* _parentBus = nullptr;
		std::vector<Bus<TObject, TListener>*> _childBuses;
	};
}
