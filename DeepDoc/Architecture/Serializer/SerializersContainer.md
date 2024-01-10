Klasa przeznaczona do przechowywania wskaźników na funkcje serializujące i deserializujące typy zmiennych. Jej funkcję ograniczają się do:
* wiązania funkcji do danego typu danych
* wywoływania funkcji serializującej dla danego typu
* wywoływania funkcji deserializującej dla danego typu

### Szablon funkcji serializującej
```  
# "VECTOR3" jest przykładowym typem do serializacji, pozostałe parametry muszą się
# zgadzać z podanym poniżej przykładem

inline YAML::Node SerializeVec3(const VECTOR3& p_value,
	SerializerContainer* p_container)  
{  
    YAML::Node node;  
    node["x"] = p_value.x;  
    node["y"] = p_value.y;  
    node["z"] = p_value.z;  
    return node;  
}  
  
inline void DeserializeVec3(const YAML::Node& p_node,
	SerializerContainer* p_container, VECTOR3& p_value)  
{  
    p_value.x = p_node["x"].as<float>();  
    p_value.y = p_node["y"].as<float>();  
    p_value.z = p_node["z"].as<float>();  
}
```