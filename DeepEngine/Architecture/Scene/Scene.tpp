namespace DeepEngine::Architecture::Scene
{
	//////////////////////////////////////
	//			ITERATOR
	//////////////////////////////////////
	
	template <typename T> requires std::is_base_of_v<SceneElement, T>
	Scene::Iterator<T>::Iterator(const Scene* p_scene): _scene(p_scene)
	{
		_currentElement = (SceneElement*)(&_scene->_sceneElements[0]);

		if (!CompareType(_currentElement))
		{
			MoveToNextElement();
		}
	}

	template <typename T> requires std::is_base_of_v<SceneElement, T>
	Scene::Iterator<T>::Iterator(const Scene* p_scene, SceneElement* p_element): _scene(p_scene), _currentElement(p_element)
	{
		// ...
	}

	template <typename T> requires std::is_base_of_v<SceneElement, T>
	Scene::Iterator<T>& Scene::Iterator<T>::operator++()
	{
		MoveToNextElement();
		return *this;
	}

	template <typename T> requires std::is_base_of_v<SceneElement, T>
	Scene::Iterator<T> Scene::Iterator<T>::operator++() const
	{
		Iterator newIterator = Iterator(_scene);
		newIterator._currentElement = _currentElement;
		newIterator.MoveToNextElement();
		return newIterator;
	}

	template <typename T> requires std::is_base_of_v<SceneElement, T>
	T* Scene::Iterator<T>::operator->() const
	{ return (T*)_currentElement; }

	template <typename T> requires std::is_base_of_v<SceneElement, T>
	T& Scene::Iterator<T>::operator*() const
	{ return *(T*)_currentElement; }

	template <typename T> requires std::is_base_of_v<SceneElement, T>
	bool Scene::Iterator<T>::operator==(const Iterator& p_other) const
	{ return _currentElement == p_other._currentElement;}

	template <typename T> requires std::is_base_of_v<SceneElement, T>
	bool Scene::Iterator<T>::CompareType(const SceneElement* p_element)
	{
		if (std::is_same_v<T, SceneElement>)
		{
			return true;
		}
		
		return p_element->IsThisType(typeid(T));
	}

	template <typename T> requires std::is_base_of_v<SceneElement, T>
	void Scene::Iterator<T>::MoveToNextElement()
	{
		if (std::is_same_v<SceneElement, T>)
		{
			_currentElement = (SceneElement*)((uint8_t*)_currentElement + _currentElement->GetSize());
			return;
		}
		
		while (true)
		{
			_currentElement = (SceneElement*)((uint8_t*)_currentElement + _currentElement->GetSize());

			if (static_cast<void*>(_currentElement) == _scene->_sceneElementsPtr)
			{
				return;
			}

			if (CompareType(_currentElement))
			{
				return;
			}
		}
	}

	template <typename T> requires std::is_base_of_v<SceneElement, T>
	Scene::Iterator<T> Scene::Begin() const
	{
		return Iterator<T>(this);
	}

	template <typename T> requires std::is_base_of_v<SceneElement, T>
	Scene::Iterator<T> Scene::End() const
	{
		return Iterator<T>(this, (SceneElement*)_sceneElementsPtr);
	}

	inline Scene::Iterator<SceneElement> Scene::Begin() const
	{
		return Iterator<SceneElement>(this);
	}

	inline Scene::Iterator<SceneElement> Scene::End() const
	{
		return Iterator<SceneElement>(this, (SceneElement*)_sceneElementsPtr);
	}
	
	//////////////////////////////////////
	//			SCENE
	//////////////////////////////////////

	inline Scene::Scene()
	{
		_sceneElementsPtr = _sceneElements;
	}

	template <typename T> requires std::is_base_of_v<SceneElement, T>
	constexpr T& Scene::CreateSceneElement()
	{
		ENGINE_TRACE("Creating object of type \"{}\" in scene at {} offset!", typeid(T).name(), _sceneElementsPtr);
		T* ptr = (T*)_sceneElementsPtr;
		memset(ptr, 0, sizeof(T));
		*ptr = T();

		SceneElement* newElement = ptr;
		newElement->_runtimeID = _elementCounter;
		newElement->_size = alignof(T);
		newElement->_typeHashCode = typeid(T).hash_code();
		newElement->_name = ptr->GetTypeName();

		_elementCounter++;
		_sceneElementsPtr = (uint8_t*)_sceneElementsPtr + ptr->GetSize();

		if (!_elementTypeToFactoryMap.contains(typeid(T).hash_code()))
		{
			auto newVector = std::vector<T*>();
			newVector.reserve(64);
			_elementTypeToFactoryMap[typeid(T).hash_code()] = newVector;
		}
		
		auto& vector = std::any_cast<std::vector<T*>&>(_elementTypeToFactoryMap[typeid(T).hash_code()]);
		vector.push_back(ptr);
		return *ptr;
	}
	
}
