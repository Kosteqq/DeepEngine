#pragma once
#define GLM_GTX_transform
#include <any>
#include <glm/glm.hpp>
#include <glm/gtx/common.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/transform2.hpp>

#include "Debugs/Logger.h"

namespace DeepEngine::Architecture::Scene
{

	struct SceneElementTypes
	{
		
	};

	struct Transform
	{
		glm::vec3 Position;
		glm::vec3 Rotation;
		glm::vec3 Scale;

		glm::mat4 GetLocalTransform() const
		{
			auto matrix = glm::mat4(1.f);
			matrix = glm::translate(matrix, Position);
			matrix = glm::rotate(matrix, Rotation.x, { 1, 0, 0 });
			matrix = glm::rotate(matrix, Rotation.y, { 0, 1, 0 });
			matrix = glm::rotate(matrix, Rotation.z, { 0, 0, 1 });
			matrix = glm::scale(matrix, Scale);
			return matrix;
		}
	};

	struct Scene;

	struct SceneElement
	{
		template <typename T>
		requires std::is_base_of_v<SceneElement, T>
		friend constexpr T& CreateSceneElement(Scene& p_scene);

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
		
		constexpr virtual const char* GetTypeName() const = 0;

	protected:
		size_t _size;
		size_t _typeHashCode;
		
		uint32_t _runtimeID;
		Transform _transform;
	};

	class Scene
	{
		template <typename T>
		requires std::is_base_of_v<SceneElement, T>
		friend constexpr T& CreateSceneElement(Scene& p_scene);

	public:
		Scene()
		{
			_sceneElementsPtr = _sceneElements;
		}

	public:
		template <typename T>
		requires std::is_base_of_v<SceneElement, T>
		class Iterator
		{
			friend class Scene;

		private:
			Iterator(const Scene* p_scene) : _scene(p_scene)
			{
				_currentElement = (SceneElement*)(&_scene->_sceneElements[0]);
				
				if (!_currentElement->IsThisType(typeid(T)))
				{
					MoveToNextElement();
				}
			}
			
			Iterator(const Scene* p_scene, SceneElement* p_element) : _scene(p_scene), _currentElement(p_element)
			{
				// ...
			}

		public:
			Iterator& operator++()
			{
				MoveToNextElement();
				return *this;
			}

			Iterator operator++() const
			{
				Iterator newIterator = Iterator(_scene);
				newIterator._currentElement = _currentElement;
				newIterator.MoveToNextElement();
				return newIterator;
			}

			T* operator->() const
			{ return (T*)_currentElement; }
			
			T& operator&() const
			{ return *(T*)_currentElement; }

			bool operator==(const Iterator& p_other) const
			{ return _currentElement == p_other._currentElement;}

		private:
			void MoveToNextElement()
			{
				while (true)
				{
					_currentElement = (SceneElement*)(((char*)_currentElement + _currentElement->GetSize()));

					if (static_cast<void*>(_currentElement) == _scene->_sceneElementsPtr)
					{
						return;
					}

					if (_currentElement->IsThisType(typeid(T)))
					{
						return;
					}
				}
			}

		private:
			const Scene* _scene;
			
			SceneElement* _currentElement;
		};

		template <typename T>
		requires std::is_base_of_v<SceneElement, T>
		Iterator<T> Begin()
		{
			return Iterator<T>(this);
		}

		template <typename T>
		requires std::is_base_of_v<SceneElement, T>
		Iterator<T> End()
		{
			return Iterator<T>(this, (SceneElement*)_sceneElementsPtr);
		}

	private:
		static const uint32_t SCENE_ELEMENT_SIZE = 8192;

		uint32_t _elementCounter = 0;
		void* _sceneElementsPtr = 0;
		void* _sceneElements[SCENE_ELEMENT_SIZE];
		std::unordered_map<uint32_t, std::any> _elementTypeToFactoryMap;
	};
	
	
	template <typename T>
	requires std::is_base_of_v<SceneElement, T>
	constexpr T& CreateSceneElement(Scene& p_scene)
	{
		ENGINE_TRACE("Creating object of type \"{}\" in scene at {} offset!", typeid(T).name(), p_scene._sceneElementsPtr);
		T* ptr = (T*)p_scene._sceneElementsPtr;
		memset(ptr, 0, sizeof(T));
		*ptr = T();

		SceneElement* newElement = ptr;
		newElement->_runtimeID = p_scene._elementCounter;
		newElement->_size = alignof(T);
		newElement->_typeHashCode = typeid(T).hash_code();
		

		p_scene._elementCounter++;
		p_scene._sceneElementsPtr = (uint8_t*)p_scene._sceneElementsPtr + ptr->GetSize();

		if (!p_scene._elementTypeToFactoryMap.contains(typeid(T).hash_code()))
		{
			auto newVector = std::vector<T*>();
			newVector.reserve(64);
			p_scene._elementTypeToFactoryMap[typeid(T).hash_code()] = newVector;
		}
		
		auto& vector = std::any_cast<std::vector<T*>&>(p_scene._elementTypeToFactoryMap[typeid(T).hash_code()]);
		vector.push_back(ptr);
		return *ptr;
	}
}
