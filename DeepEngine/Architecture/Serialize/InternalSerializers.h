#pragma once
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

namespace DeepEngine::Architecture::Internal
{

	template <typename T>
	YAML::Node SerializeValue(const T& p_value);
	

	/////////////////////////////////////////////
	/// GLM
	/////////////////////////////////////////////

	template <>
	YAML::Node SerializeValue(const glm::vec1& p_value)
	{
		YAML::Node node;
		node.SetTag("glm@vec1");
		node["x"] = p_value.x;
		return node;
	}

	template <>
	YAML::Node SerializeValue(const glm::vec2& p_value)
	{
		YAML::Node node;
		node.SetTag("glm@vec2");
		node["x"] = p_value.x;
		node["y"] = p_value.y;
		return node;
	}

	template <>
	YAML::Node SerializeValue(const glm::vec3& p_value)
	{
		YAML::Node node;
		node.SetTag("glm@vec3");
		node["x"] = p_value.x;
		node["y"] = p_value.y;
		node["z"] = p_value.z;
		return node;
	}

	template <>
	YAML::Node SerializeValue(const glm::vec4& p_value)
	{
		YAML::Node node;
		node.SetTag("glm@vec4");
		node["x"] = p_value.x;
		node["y"] = p_value.y;
		node["z"] = p_value.z;
		node["w"] = p_value.w;
		return node;
	}
	
}
