#include <stack>

#include "Debugs/Logger.h"
#include "Debugs/InitializationTracker.h"
#include "Architecture/EngineSystem.h"
#include "Debugs/InitializationMilestone.h"
#include "Debugs/Timing.h"
#include "Renderer/RendererSubsystem.h"
#include "Window/WindowSubsystem.hpp"
#include "VulkanPrototype/VulkanPrototype.h"

#include "Architecture/EventBus/BaseEvent.h"
#include "Architecture/EventBus/EventListener.h"
#include "Architecture/EventBus/EventBus.h"

BEGIN_LOCAL_EVENT_DEFINITION(LocalTestEvent)
    uint32_t TestInt;
END_EVENT_DEFINITION

BEGIN_GLOBAL_EVENT_DEFINITION(GlobalTestEvent)
    uint32_t TestInt;
END_EVENT_DEFINITION


int main(int argc, char* argv[])
{
    DeepEngine::Debug::Logger::Initialize("Logs/engine.log");
    // auto mainBus = DeepEngine::Architecture::EventBus();
    // auto listener = mainBus.CreateListener<GlobalTestEvent>();
    //
    // listener->BindCallback(OnTestCallback);
    // listener->BindCallback(BlockTestCallback);
    // listener->BindCallback(OnTestCallback);
    
    // auto globalEvent = GlobalTestEvent();
    // globalEvent.TestInt = 20;
    // mainBus.Publish(globalEvent);
    
    {
        TIMER("Main");

        DEFINE_MILESTONE(FailedMilestone);
        DEFINE_MILESTONE(FulfiledMilestone);
        
        FULFIL_MILESTONE(FulfiledMilestone);
        FAIL_MILESTONE(FailedMilestone);
        
        ENGINE_INFO("Hello World");

        auto subsystemsManager = DeepEngine::Architecture::EngineSubsystemsManager();
        auto windowSubsystem = subsystemsManager.CreateSubsystem<DeepEngine::WindowSubsystem>(800, 600, "1800 lines for fucking triangle (:");
    	subsystemsManager.CreateSubsystem<DeepEngine::Renderer::RendererSubsystem>();

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
