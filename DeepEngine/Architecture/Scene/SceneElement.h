#pragma once
#include <cstdint>
#include <typeinfo>
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace DeepEngine::Architecture::Scene
{

	// TODO(Kostek): Move to internal engine types
	struct Transform
	{
		glm::vec3 Position;
		glm::vec3 Rotation;
		glm::vec3 Scale;

		glm::mat4 GetLocalTransform() const;
	};

	class Scene;

	class SceneElement
	{
		friend class Scene;

	protected:
		SceneElement() = default;

	public:
		virtual ~SceneElement() = default;
		
		// maybe use https://github.com/mariusbancila/stduuid
		// uint64_t GUID;

		bool IsThisType(const std::type_info& p_typeInfo) const
		{ return _typeHashCode == p_typeInfo.hash_code(); }

		constexpr uint32_t RuntimeID() const
		{ return _runtimeID; }

		constexpr Transform& GetTransform()
		{ return _transform; }

		constexpr size_t GetSize() const
		{ return _size * 8; }

		constexpr const char* GetName() const
		{ return _name; }

		constexpr virtual const char* GetTypeName() const = 0;

	protected:
		size_t _size;
		size_t _typeHashCode;
		const char* _name;

		uint32_t _runtimeID;
		Transform _transform;
	};
	
}
