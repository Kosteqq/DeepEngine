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

class YamlSerializeDescription
{
public:
    struct ElementMetadata
    {
        const char* MemberName;
        size_t MemberOffset;
    };

public:
    constexpr virtual const char* GetName() const = 0;
    constexpr virtual const std::vector<ElementMetadata>& GetElements() const = 0;
};

// Friend some class, add GetSerializeDescription func
#define YAML_SERIALIZABLE(Class, ...)                                           \
class Class## YamlSerializeDescription final : YamlSerializeDescription {       \
    using SerializableClass = Class;                                            \
public:                                                                         \
    constexpr const char* GetName() const override                              \
    { return #Class; }                                                          \
    constexpr const std::vector<ElementMetadata>& GetElements() const override  \
    { return _elements; }                                                       \
private:                                                                        \
    std::vector<ElementMetadata> _elements {                                    \
        __VA_ARGS__                                                             \
    };                                                                          \
};

#define SERIALIZE_MEMBER(MemberName) ElementMetadata { #MemberName, offsetof(SerializableClass, MemberName) }


class TestConfig
{
    YAML_SERIALIZABLE(TestConfig,
        SERIALIZE_MEMBER(x),
        SERIALIZE_MEMBER(x)
    )

    void GetDescription();

public:
    void BeforeSerialize()
    {
        typedef void(TestConfig::*funcPointer)();
        std::function<void(TestConfig*)> q = [w = &TestConfig::BeforeSerialize](TestConfig* dupa){ (dupa->*w)(); };
    }

private:
    int x;
};



int main(int p_argc, char* p_argv[])
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
