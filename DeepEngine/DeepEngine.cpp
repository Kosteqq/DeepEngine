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

#include "Architecture/Scene/Scene.h"
#include "Architecture/Serialize/SerializersContainer.h"
#include "Architecture/Serialize/DefaultImplementations/GlmSerializers.h"



struct MyCustomSceneElement final : DeepEngine::Architecture::Scene::SceneElement
{
    constexpr const char* GetTypeName() const override
    { return "MyCustomSceneElement"; }
};

struct MyCustomSecondSceneElement final : DeepEngine::Architecture::Scene::SceneElement
{
    constexpr const char* GetTypeName() const override
    { return "MyCustomSecondSceneElement"; }
};

void TestSerializer();

int main(int p_argc, char* p_argv[])
{    
    DeepEngine::Debug::Logger::Initialize("Logs/engine.log");
    auto engineEventBus = DeepEngine::Architecture::EventBus();

    DeepEngine::Architecture::Scene::Scene scene;
    
    DeepEngine::Architecture::Scene::CreateSceneElement<MyCustomSecondSceneElement>(scene);
    DeepEngine::Architecture::Scene::CreateSceneElement<MyCustomSecondSceneElement>(scene);
    DeepEngine::Architecture::Scene::CreateSceneElement<MyCustomSceneElement>(scene);
    DeepEngine::Architecture::Scene::CreateSceneElement<MyCustomSecondSceneElement>(scene);
    DeepEngine::Architecture::Scene::CreateSceneElement<MyCustomSceneElement>(scene);

    for (auto it = scene.Begin<MyCustomSceneElement>(); it != scene.End<MyCustomSceneElement>(); ++it)
    {
        std::cout << it->GetTypeName() << std::endl;
    }
    for (auto it = scene.Begin<MyCustomSecondSceneElement>(); it != scene.End<MyCustomSecondSceneElement>(); ++it)
    {
        std::cout << it->GetTypeName() << std::endl;
    }

    TestSerializer();
    
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

void TestSerializer()
{
    TIMER("Testing serializer");
    DeepEngine::Architecture::SerializerContainer serializeContainer;
    DeepEngine::Architecture::SerializeInternal::BindGlmSerializers(serializeContainer);

    auto vector = glm::vec3 { 0.2f, 20.0f, 10.0f };
    auto matrix = glm::mat4x4{};
    
    YAML::Emitter out;

    out << YAML::BeginSeq;
    {
        TIMER("Serializing");
        out << serializeContainer.InvokeSerializeFunc(vector);
        out << serializeContainer.InvokeSerializeFunc(matrix);
    }
    out << YAML::EndSeq;

    std::ofstream file;

    {
        TIMER("Writing to file");
        file.open("TestFile.txt", std::ios::out | std::ios::ate);
        file << out.c_str();
        file.close();
    }
}