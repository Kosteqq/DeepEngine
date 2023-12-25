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


int main(int argc, char* argv[])
{
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
