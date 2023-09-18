#include "Architecture/EngineSystem.h"
#include "Debugs/Logger.h"
#include "Window/WindowSubsystem.hpp"


class TestSubsystem : DeepEngine::Architecture::EngineSubsystem
{
public:
    TestSubsystem(int x)
    {
        
    }

protected:
    bool Init() override
    {
        ENGINE_INFO("INIT");
        return true;
    }

    void Destroy() override
    {
        ENGINE_INFO("Destroy");
    }

    void Tick() override
    { }
};

int main(int argc, char* argv[])
{
    DeepEngine::Core::Debug::Logger::Initialize("Logs/engine.log");
    ENGINE_INFO("Hello World");
    ENGINE_TRACE("FUCK UNITY :>");
    ENGINE_DEBUG("FUCK UNITY :>");
    ENGINE_INFO("FUCK UNITY :>");
    ENGINE_WARN("FUCK UNITY :>");
    ENGINE_ERR("FUCK UNITY :>");

    // Logger x = Logger();

    auto subsystemsManager = DeepEngine::Architecture::EngineSubsystemsManager();
    subsystemsManager.CreateSubsystem<TestSubsystem>(2);
    subsystemsManager.CreateSubsystem<DeepEngine::WindowSubsystem>(600, 800, "fckUnity");

    if (!subsystemsManager.Init())
    {
        ENGINE_INFO("Failed to initialize subsystems!!");
        return -1;
    }
    
    return 0;
}
