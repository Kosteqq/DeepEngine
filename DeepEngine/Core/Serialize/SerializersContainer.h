#pragma once
#include <any>
#include <typeindex>
#include <yaml-cpp/yaml.h>

#include "Debug/Logger.h"

namespace DeepEngine::Core::Serialize
{
	
	class SerializerContainer
	{
	public:
		template <class T>
		using SerializeFuncPtr = YAML::Node (*)(const T& p_value, SerializerContainer* p_container);
		
		template <class T>
		using DeserializeFuncPtr = void (*)(const YAML::Node& p_serializedNode, SerializerContainer* p_container, T& p_value); 
		
		template <class T>
		void Bind(SerializeFuncPtr<T> p_serializeFunction, DeserializeFuncPtr<T> p_deserializeFunction)
		{
			_typeToBindLookup[typeid(T)] = FunctionBind<T> { p_serializeFunction, p_deserializeFunction };
		}

		template <class T>
		YAML::Node InvokeSerializeFunc(const T& p_value)
		{
			if (!_typeToBindLookup.contains(typeid(T)))
			{
				ENGINE_ERR("There is no bound serialize funcs for type \"{}\"", typeid(T).name());
				return YAML::Node { };
			}

			const SerializeFuncPtr<T> funcPtr = std::any_cast<FunctionBind<T>>(_typeToBindLookup[typeid(T)]).SerializeFuncPtr;
			return funcPtr(p_value, this);
		}

		template <class T>
		void InvokeDeserializeFunc(const YAML::Node& p_serializedNode, T& p_value)
		{ 
			if (!_typeToBindLookup.contains(typeid(T)))
			{
				ENGINE_ERR("There is no bound serialize funcs for type \"{}\"", typeid(T).name());
				return;
			}

			const DeserializeFuncPtr<T> funcPtr = std::any_cast<FunctionBind<T>>(_typeToBindLookup[typeid(T)]).DeserializeFuncPtr;
			funcPtr(p_serializedNode, this, p_value);
		}

	private:
		template <class T>
		struct FunctionBind
		{
			SerializeFuncPtr<T> SerializeFuncPtr;
			DeserializeFuncPtr<T> DeserializeFuncPtr;
		};
		
		std::unordered_map<std::type_index, std::any> _typeToBindLookup;
	};
	
}
