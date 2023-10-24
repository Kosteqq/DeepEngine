#include "Debugs/Logger.h"
#include "Debugs/InitializationTracker.h"
#include "Architecture/EngineSystem.h"
#include "Debugs/InitializationMilestone.h"
#include "Debugs/Timing.h"
#include "Window/WindowSubsystem.hpp"
#include "VulkanPrototype/VulkanPrototype.h"


class TestSubsystem : public DeepEngine::Architecture::EngineSubsystem
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
    {
        TIMER("Main");
        DeepEngine::Debug::Logger::Initialize("Logs/engine.log");

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

        auto subsystemsManager = DeepEngine::Architecture::EngineSubsystemsManager();
        subsystemsManager.CreateSubsystem<TestSubsystem>(2);
        auto windowSubsystem = subsystemsManager.CreateSubsystem<DeepEngine::WindowSubsystem>(800, 600, "1800 lines for fucking triangle (:");
    	subsystemsManager.CreateSubsystem<DeepEngine::Renderer::VulkanPrototype>();

        if (!subsystemsManager.Init())
        {
            ENGINE_INFO("Failed to initialize subsystems!!");
            return -1;
        }

        while (true)
        {
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
