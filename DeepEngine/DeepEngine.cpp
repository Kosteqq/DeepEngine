#include "Debug/Debug.h"
#include "Architecture/EngineSystem.h"
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
        LOG("INIT")
        return true;
    }

    void Destroy() override
    {
        LOG("Destroy")
    }

    void Tick() override
    { }
};

int main(int argc, char* argv[])
{
    LOG("Hello World")

    auto subsystemsManager = DeepEngine::Architecture::EngineSubsystemsManager();
    subsystemsManager.CreateSubsystem<TestSubsystem>(2);
    subsystemsManager.CreateSubsystem<DeepEngine::WindowSubsystem>(600, 800, "fckUnity");

    if (!subsystemsManager.Init())
    {
        LOG("Failed to initialize subsystems!!")
        return -1;
    }
    
    return 0;
}
