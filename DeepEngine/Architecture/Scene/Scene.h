#pragma once
#define GLM_GTX_transform
#include <any>
#include <glm/gtx/hash.hpp>

#include "SceneElement.h"

namespace DeepEngine::Architecture::Scene
{

	class Scene
	{
	public:
		Scene();

		template <typename T>
		requires std::is_base_of_v<SceneElement, T>
		constexpr T& CreateSceneElement();

	public:
		template <typename T>
		requires std::is_base_of_v<SceneElement, T>
		class Iterator
		{
			friend class Scene;

		private:
			Iterator(const Scene* p_scene);
			Iterator(const Scene* p_scene, SceneElement* p_element);

		public:
			Iterator& operator++();
			Iterator operator++() const;

			T* operator->() const;
			T& operator*() const;

			bool operator==(const Iterator& p_other) const;

		private:
			static bool CompareType(const SceneElement* p_element);
			void MoveToNextElement();

		private:
			const Scene* _scene;
			SceneElement* _currentElement;
		};

		template <typename T>
		requires std::is_base_of_v<SceneElement, T>
		Iterator<T> Begin() const;

		template <typename T>
		requires std::is_base_of_v<SceneElement, T>
		Iterator<T> End() const;

		Iterator<SceneElement> Begin() const;
		Iterator<SceneElement> End() const;

	private:
		static const uint32_t SCENE_ELEMENT_SIZE = 8192;

		uint32_t _elementCounter = 0;
		void* _sceneElementsPtr = 0;
		void* _sceneElements[SCENE_ELEMENT_SIZE];
		std::unordered_map<uint32_t, std::any> _elementTypeToFactoryMap;
	};
	
}

#include "Scene.tpp"

