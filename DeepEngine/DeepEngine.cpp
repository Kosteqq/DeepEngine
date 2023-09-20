#include "Debugs/Logger.h"
#include "Debugs/InitializationTracker.h"
#include "Architecture/EngineSystem.h"
#include "Debugs/InitializationMilestone.h"
#include "VulkanPrototype/VulkanPrototype.h"
#include "Window/WindowSubsystem.hpp"


class TestSubsystem : public DeepEngine::Core::Architecture::EngineSubsystem
{
public:
    TestSubsystem(int x) : EngineSubsystem("Test Subsystem")
    {
        INFO("Constructor {0}", x);
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

    DEFINE_MILESTONE(FailedMilestone);
    DEFINE_MILESTONE(FulfiledMilestone);
    
    FULFIL_MILESTONE(FulfiledMilestone);
    FAIL_MILESTONE(FailedMilestone);
    
    ENGINE_INFO("Hello World");
    ENGINE_TRACE("TRACE");
    ENGINE_DEBUG("DEBUG");
    ENGINE_INFO("INFO");
    ENGINE_WARN("WARNING");
    ENGINE_ERR("ERROR");

    auto subsystemsManager = DeepEngine::Core::Architecture::EngineSubsystemsManager();
    subsystemsManager.CreateSubsystem<TestSubsystem>(2);
    subsystemsManager.CreateSubsystem<DeepEngine::WindowSubsystem>(800, 600, "fckUnity");
    subsystemsManager.CreateSubsystem<DeepEngine::Renderer::VulkanPrototype>();

    if (!subsystemsManager.Init())
    {
        ENGINE_INFO("Failed to initialize subsystems!!");
        return -1;
    }

    while (1)
    {
        subsystemsManager.Tick();
    }

    return 0;
}
