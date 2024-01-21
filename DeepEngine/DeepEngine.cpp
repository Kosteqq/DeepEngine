#include <fstream>
#include <set>
#include <stack>

#include "Debug/Logger.h"
#include "Debug/InitializationTracker.h"
#include "Core/EngineSystem.h"
#include "Debug/InitializationMilestone.h"
#include "Debug/Timing.h"
#include "Engine/Renderer/RendererSubsystem.h"
#include "Engine/Window/WindowSubsystem.hpp"

#include "Core/Events/EventBus.h"

#include <yaml-cpp/yaml.h>

#include "Core/Scene/Scene.h"
#include "Core/Serialize/SerializersContainer.h"
#include "Core/Serialize/DefaultImplementations/GlmSerializers.h"

using namespace DeepEngine;

struct MyCustomSceneElement final : Core::Scene::SceneElement
{
    constexpr const char* GetTypeName() const override
    { return "MyCustomSceneElement"; }
};

struct MyCustomSecondSceneElement final : Core::Scene::SceneElement
{
    constexpr const char* GetTypeName() const override
    { return "MyCustomSecondSceneElement"; }
};

void TestSerializer();

int main(int p_argc, char* p_argv[])
{    
    Debug::Logger::Initialize("Logs/engine.log");
    auto engineEventBus = Core::Events::EventBus();

    Core::Scene::Scene scene;

    scene.CreateSceneElement<MyCustomSecondSceneElement>();
    scene.CreateSceneElement<MyCustomSecondSceneElement>();
    scene.CreateSceneElement<MyCustomSceneElement>();
    scene.CreateSceneElement<MyCustomSecondSceneElement>();
    scene.CreateSceneElement<MyCustomSceneElement>();

    for (auto it = scene.Begin<MyCustomSceneElement>(); it != scene.End<MyCustomSceneElement>(); ++it)
    {
        std::cout << it->GetName() << std::endl;
    }
    for (auto it = scene.Begin<MyCustomSecondSceneElement>(); it != scene.End<MyCustomSecondSceneElement>(); ++it)
    {
        std::cout << it->GetName() << std::endl;
    }
    for (auto it = scene.Begin(); it != scene.End(); ++it)
    {
        std::cout << it->GetName() << std::endl;
    }

    TestSerializer();
    
    {
        TIMER("Main");

        DEFINE_MILESTONE(FailedMilestone);
        DEFINE_MILESTONE(FulfiledMilestone);
        
        FULFIL_MILESTONE(FulfiledMilestone);
        FAIL_MILESTONE(FailedMilestone);
        
        ENGINE_INFO("Hello World");

        auto subsystemsManager = Core::EngineSubsystemsManager(engineEventBus);
        auto windowSubsystem = subsystemsManager.CreateSubsystem<WindowSubsystem>(800, 600, "1800 lines for fucking triangle (:");
    	subsystemsManager.CreateSubsystem<Engine::Renderer::RendererSubsystem>();

        if (!subsystemsManager.Init())
        {
            ENGINE_INFO("Failed to initialize subsystems!!");
            return -1;
        }

        while (true)
        {
            TIMER("Tick");
            
            subsystemsManager.Tick(scene);
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
    Core::Serialize::SerializerContainer serializeContainer;
    Core::Serialize::Internal::BindGlmSerializers(serializeContainer);

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