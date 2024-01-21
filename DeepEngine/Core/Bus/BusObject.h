#pragma once

namespace DeepEngine::Core::Bus
{

	struct BusObject
	{
		BusObject(const BusObject& p_other) = delete;
		BusObject(BusObject&& p_other) = delete;
    
		BusObject() = default;
		virtual ~BusObject() = default;
    
		constexpr virtual const char* GetName() const = 0;
		constexpr virtual size_t GetTypeID() const = 0;
	};
	
}
