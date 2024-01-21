#pragma once
#include <yaml-cpp/yaml.h>

#include "Core/Serialize/SerializersContainer.h"

namespace DeepEngine::Core::Serialize::Internal
{

	using Serialize::SerializerContainer;

	inline YAML::Node SerializeVec1(const glm::vec1& p_value, SerializerContainer* p_container)
	{
		YAML::Node node;
		node["x"] = p_value.x;
		return node;
	}

	inline void DeserializeVec1(const YAML::Node& p_node, SerializerContainer* p_container, glm::vec1& p_value)
	{
		p_value.x = p_node["x"].as<float>();
	}

	inline YAML::Node SerializeVec2(const glm::vec2& p_value, SerializerContainer* p_container)
	{
		YAML::Node node;
		node["x"] = p_value.x;
		node["y"] = p_value.y;
		return node;
	}

	inline void DeserializeVec2(const YAML::Node& p_node, SerializerContainer* p_container, glm::vec2& p_value)
	{
		p_value.x = p_node["x"].as<float>();
		p_value.y = p_node["y"].as<float>();
	}

	inline YAML::Node SerializeVec3(const glm::vec3& p_value, SerializerContainer* p_container)
	{
		YAML::Node node;
		node["x"] = p_value.x;
		node["y"] = p_value.y;
		node["z"] = p_value.z;
		return node;
	}

	inline void DeserializeVec3(const YAML::Node& p_node, SerializerContainer* p_container, glm::vec3& p_value)
	{
		p_value.x = p_node["x"].as<float>();
		p_value.y = p_node["y"].as<float>();
		p_value.z = p_node["z"].as<float>();
	}

	inline YAML::Node SerializeVec4(const glm::vec4& p_value, SerializerContainer* p_container)
	{
		YAML::Node node;
		node["x"] = p_value.x;
		node["y"] = p_value.y;
		node["z"] = p_value.z;
		node["w"] = p_value.w;
		return node;
	}

	inline void DeserializeVec4(const YAML::Node& p_node, SerializerContainer* p_container, glm::vec4& p_value)
	{
		p_value.x = p_node["x"].as<float>();
		p_value.y = p_node["y"].as<float>();
		p_value.z = p_node["z"].as<float>();
		p_value.w = p_node["w"].as<float>();
	}

	inline YAML::Node SerializeMat4x4(const glm::mat4x4& p_value, SerializerContainer* p_container)
	{
		YAML::Node node;

		node["row_0"] =  p_container->InvokeSerializeFunc<glm::vec4>(p_value[0]);
		node["row_1"] =  p_container->InvokeSerializeFunc<glm::vec4>(p_value[1]);
		node["row_2"] =  p_container->InvokeSerializeFunc<glm::vec4>(p_value[2]);
		node["row_3"] =  p_container->InvokeSerializeFunc<glm::vec4>(p_value[3]);

		return node;
	}

	inline void DeserializeMat4x4(const YAML::Node& p_node, SerializerContainer* p_container, glm::mat4x4& p_value)
	{
		p_container->InvokeDeserializeFunc<glm::vec4>(p_node["row_0"], p_value[0]);
		p_container->InvokeDeserializeFunc<glm::vec4>(p_node["row_1"], p_value[1]);
		p_container->InvokeDeserializeFunc<glm::vec4>(p_node["row_2"], p_value[2]);
		p_container->InvokeDeserializeFunc<glm::vec4>(p_node["row_3"], p_value[3]);
	}

	inline void BindGlmSerializers(SerializerContainer& p_container)
	{
		p_container.Bind<glm::vec1>(&SerializeVec1, &DeserializeVec1);
		p_container.Bind<glm::vec2>(&SerializeVec2, &DeserializeVec2);
		p_container.Bind<glm::vec3>(&SerializeVec3, &DeserializeVec3);
		p_container.Bind<glm::vec4>(&SerializeVec4, &DeserializeVec4);
		p_container.Bind<glm::mat4x4>(&SerializeMat4x4, &DeserializeMat4x4);
	}
	
}
