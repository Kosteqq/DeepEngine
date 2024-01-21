#pragma once
#include "Core/Bus/BusObject.h"

#define BEGIN_GLOBAL_EVENT_DEFINITION(Name)                                                 \
	struct Name final : DeepEngine::Core::Events::BaseEvent {                               \
		constexpr const char* GetName() const override                                      \
		{ return #Name; }                                                                   \
		constexpr DeepEngine::Core::Events::EventScope GetPublishingScope() const override  \
		{ return DeepEngine::Core::Events::EventScope::GLOBAL; }                            \
		size_t GetTypeID() const override                                                   \
		{ return typeid(Name).hash_code(); }

#define BEGIN_LOCAL_EVENT_DEFINITION(Name)                                                  \
	struct Name final : DeepEngine::Core::Events::BaseEvent {     \
		constexpr const char* GetName() const override                                      \
		{ return #Name; }                                                                   \
		constexpr DeepEngine::Core::Events::EventScope GetPublishingScope() const override  \
		{ return DeepEngine::Core::Events::EventScope::LOCAL; }                             \
		size_t GetTypeID() const override                                                   \
		{ return typeid(Name).hash_code(); }

#define END_EVENT_DEFINITION                                                                \
	};

namespace DeepEngine::Core::Events
{
	
	enum class EventScope
	{
		LOCAL = 0,
		GLOBAL = 1
	};

	struct BaseEvent : Bus::BusObject
	{
		constexpr virtual EventScope GetPublishingScope() const = 0;
	};
	
}
