#include <fstream>
#include <set>
#include <stack>

#include "Debugs/Logger.h"
#include "Debugs/InitializationTracker.h"
#include "Architecture/EngineSystem.h"
#include "Debugs/InitializationMilestone.h"
#include "Debugs/Timing.h"
#include "Renderer/RendererSubsystem.h"
#include "Window/WindowSubsystem.hpp"

#include "Architecture/EventBus/EventBus.h"

#include <yaml-cpp/yaml.h>
#include "Architecture/Serialize/InternalSerializers.h"

class SerializersContainer
{
private:
    template <typename T>
    struct SerializeBind
    {
        const char* TypeName = typeid(T).name();
        std::function<YAML::Node (const T&)> SerializeFunc;
        std::function<void(const YAML::Node&, T&)> DeserializeFunc;

        SerializeBind(std::function<YAML::Node (const T&)>&& p_serializeFunc,
            std::function<void(const YAML::Node&, T&)>&& p_deserializeFunc,
            SerializersContainer* p_container)
        {
            SerializeFunc = p_serializeFunc;
            DeserializeFunc = p_deserializeFunc;
            p_container->_serializers[typeid(T)] = *this;
        }
    };
    

private:
    std::unordered_map<std::type_index, std::any> _serializers;
    
public:
    void Init()
    {
        
    }

    template <typename TType>
    YAML::Node Serialize(const TType& p_value)
    {
        if (!_serializers.contains(typeid(TType)))
        {
            return YAML::Node { };
        }

        return std::any_cast<SerializeBind<TType>>(_serializers[typeid(TType)]).SerializeFunc(p_value);
    }
    
private:
    static YAML::Node Vec3Serialize(const glm::vec3& p_vec)
    {
        YAML::Node node;
        node["x"] = p_vec.x;
        node["y"] = p_vec.y;
        node["z"] = p_vec.z;

        return node;
    }

    static void Vec3Deserialize(const YAML::Node& p_node, glm::vec3& p_value)
    {
        p_value.x = p_node["x"].as<float>();
        p_value.y = p_node["y"].as<float>();
        p_value.z = p_node["z"].as<float>();
    }
    SerializeBind<glm::vec3> _vec3Bind { &SerializersContainer::Vec3Serialize, &SerializersContainer::Vec3Deserialize, this };
};


int main(int p_argc, char* p_argv[])
{
    SerializersContainer container;
    
    YAML::Emitter out;

    out << YAML::BeginSeq;
    out << DeepEngine::Architecture::Internal::SerializeValue(glm::vec3 { 0.2f, 20.0f, 10.0f });
    out << container.Serialize(glm::vec3 { 0.2f, 20.0f, 10.0f });
    out << YAML::EndSeq;

    std::ofstream file;
    file.open("TestConfig.txt", std::ios::out | std::ios::ate);
    file << out.c_str();
    file.close();
    
    DeepEngine::Debug::Logger::Initialize("Logs/engine.log");
    auto engineEventBus = DeepEngine::Architecture::EventBus();
    
    {
        TIMER("Main");

        DEFINE_MILESTONE(FailedMilestone);
        DEFINE_MILESTONE(FulfiledMilestone);
        
        FULFIL_MILESTONE(FulfiledMilestone);
        FAIL_MILESTONE(FailedMilestone);
        
        ENGINE_INFO("Hello World");

        auto subsystemsManager = DeepEngine::Architecture::EngineSubsystemsManager(engineEventBus);
        auto windowSubsystem = subsystemsManager.CreateSubsystem<DeepEngine::WindowSubsystem>(800, 600, "1800 lines for fucking triangle (:");
    	subsystemsManager.CreateSubsystem<DeepEngine::Renderer::RendererSubsystem>();

        if (!subsystemsManager.Init())
        {
            ENGINE_INFO("Failed to initialize subsystems!!");
            return -1;
        }

        while (true)
        {
            TIMER("Tick");
            
            subsystemsManager.Tick();
            if (windowSubsystem->WantsToExit())
            {
                break;
            }
        }
    }

    PRINT_TIMER_SUMMARY();
    return 0;
}
