#pragma once
#include <typeinfo>

#include "Architecture/Internal/BusObject.h"

#define BEGIN_GLOBAL_EVENT_DEFINITION(Name)                                                 \
	struct Name final : DeepEngine::Architecture::BaseEvent {                               \
		constexpr const char* GetName() const override                                      \
		{ return #Name; }                                                                   \
		constexpr DeepEngine::Architecture::EventScope GetPublishingScope() const override  \
		{ return DeepEngine::Architecture::EventScope::GLOBAL; }                            \
		size_t GetTypeID() const override                                                   \
		{ return typeid(Name).hash_code(); }

#define BEGIN_LOCAL_EVENT_DEFINITION(Name)                                                  \
	struct Name final : DeepEngine::Architecture::BaseEvent {     \
		constexpr const char* GetName() const override                                      \
		{ return #Name; }                                                                   \
		constexpr DeepEngine::Architecture::EventScope GetPublishingScope() const override  \
		{ return DeepEngine::Architecture::EventScope::LOCAL; }                             \
		size_t GetTypeID() const override                                                   \
		{ return typeid(Name).hash_code(); }

#define END_EVENT_DEFINITION                                                                \
	};

namespace DeepEngine::Architecture
{
	
	enum class EventScope
	{
		LOCAL = 0,
		GLOBAL = 1
	};

	struct BaseEvent : Internal::BusObject
	{
		constexpr virtual EventScope GetPublishingScope() const = 0;
	};
	
}
