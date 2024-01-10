#pragma once
#include <any>
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

namespace DeepEngine::Architecture::Internal
{

	template <typename T>
	YAML::Node SerializeValue(const T& p_value);

	template <typename T>
	void DeserializeVec1(const YAML::Node& p_node, T& p_deserializedValue);
	

	/////////////////////////////////////////////
	/// GLM
	/////////////////////////////////////////////

	inline YAML::Node SerializeVec1(const glm::vec1& p_value)
	{
		YAML::Node node;
		node["x"] = p_value.x;
		return node;
	}

	inline void DeserializeVec1(const YAML::Node& p_node, glm::vec1& p_value)
	{
		p_value.x = p_node["x"].as<float>();
	}

	template <>
	inline YAML::Node SerializeValue(const glm::vec2& p_value)
	{
		YAML::Node node;
		node.SetTag("glm@vec2");
		node["x"] = p_value.x;
		node["y"] = p_value.y;
		return node;
	}

	template <>
	inline void DeserializeVec1(const YAML::Node& p_node, glm::vec2& p_value)
	{
		p_value.x = p_node["x"].as<float>();
		p_value.y = p_node["y"].as<float>();
	}

	template <>
	inline YAML::Node SerializeValue(const glm::vec3& p_value)
	{
		YAML::Node node;
		node.SetTag("glm@vec3");
		node["x"] = p_value.x;
		node["y"] = p_value.y;
		node["z"] = p_value.z;
		return node;
	}

	template <>
	inline void DeserializeVec1(const YAML::Node& p_node, glm::vec3& p_value)
	{
		p_value.x = p_node["x"].as<float>();
		p_value.y = p_node["y"].as<float>();
		p_value.z = p_node["z"].as<float>();
	}
	
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

	class Serializer
	{
	public:
		void Serialize();
		void Deserialize();
	};

	inline YAML::Node SerializeValue(const glm::vec4& p_value, SerializerContainer* p_container)
	{
		YAML::Node node;
		node["x"] = p_value.x;
		node["y"] = p_value.y;
		node["z"] = p_value.z;
		node["w"] = p_value.w;
		return node;
	}

	inline void Deserialize(const YAML::Node& p_node,
			SerializerContainer* p_container, glm::vec4& p_value)
	{
		p_value.x = p_node["x"].as<float>();
		p_value.y = p_node["y"].as<float>();
		p_value.z = p_node["z"].as<float>();
		p_value.w = p_node["w"].as<float>();
		auto x = glm::vec1 { 0.f};
		p_container->InvokeDeserializeFunc(p_node[2], x);
	}
	
}
