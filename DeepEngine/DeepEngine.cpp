#include "Debugs/Logger.h"
#include "Debugs/InitializationTracker.h"
#include "Architecture/EngineSystem.h"
#include "Debugs/InitializationMilestone.h"
#include "Debugs/Timing.h"
#include "Renderer/RendererSubsystem.h"
#include "Window/WindowSubsystem.hpp"
#include "VulkanPrototype/VulkanPrototype.h"

#include <vector>
#include <typeinfo>

enum PublishingScope
{
    INHERIT = 0,
    LOCAL = 1,
    GLOBAL = 2
};

class BaseEvent
{
public:
    BaseEvent(const BaseEvent& p_other) = delete;
    BaseEvent(BaseEvent&& p_other) = delete;
    
    BaseEvent() = default;
    virtual ~BaseEvent() = default;
    
    constexpr virtual const char* GetName() const = 0;
    constexpr virtual PublishingScope GetPublishingScope() const = 0;
    constexpr virtual size_t GetEventTypeID() const = 0;
};

#define BEGIN_GLOBAL_EVENT_DEFINITION(Name)                             \
    struct Name final : BaseEvent {                                     \
        constexpr const char* GetName() const override                  \
        { return #Name; }                                               \
        constexpr PublishingScope GetPublishingScope() const override   \
        { return PublishingScope::GLOBAL; }                             \
        size_t GetEventTypeID() const override                          \
        { return typeid(Name).hash_code(); }

#define BEGIN_LOCAL_EVENT_DEFINITION(Name)                              \
    struct Name final : BaseEvent {                                     \
        constexpr const char* GetName() const override                  \
        { return #Name; }                                               \
        constexpr PublishingScope GetPublishingScope() const override   \
        { return PublishingScope::LOCAL; }                              \
        size_t GetEventTypeID() const override                          \
        { return typeid(Name).hash_code(); }

#define END_EVENT_DEFINITION                                            \
    };

BEGIN_LOCAL_EVENT_DEFINITION(LocalTestEvent)
    uint32_t TestInt;
END_EVENT_DEFINITION

BEGIN_GLOBAL_EVENT_DEFINITION(GlobalTestEvent)
    uint32_t TestInt;
END_EVENT_DEFINITION

enum PublishingResult
{
    PASS = 0,   // just continue
    
    SKIP = 1 << 0,   // skipping other callbacks in listener
    BLOCK_GLOBAL = 1 << 1, // block other listeners declared in buses above
    BLOCK_LOCAL = 1 << 2, // block other listeners declared in local buses (same or below)
    
    BLOCK = SKIP | BLOCK_GLOBAL | BLOCK_LOCAL,  // just blocking
};

class EventBus;

class BaseEventListener
{
protected:
    BaseEventListener(std::function<void(BaseEventListener*)>&& p_onDestroyCallback)
        : _onDestroyCallback(p_onDestroyCallback)
    { }

public:
    virtual ~BaseEventListener()
    {
        _onDestroyCallback(this);
    }

    virtual PublishingResult Publish(const BaseEvent* p_event) = 0;

    constexpr virtual size_t GetListeningEventType() const = 0;

private:
    const std::function<void(BaseEventListener*)> _onDestroyCallback;
};

template <typename TEvent>
requires std::is_base_of_v<BaseEvent, TEvent>
class EventListener final : BaseEventListener
{
    friend class EventBus;

public:
     EventListener(std::function<void(BaseEventListener*)> p_onDestroyCallback)
         : BaseEventListener(std::move(p_onDestroyCallback))
     {
         _callbacks.reserve(16);
     }
    
    ~EventListener() override = default;

    PublishingResult Publish(const BaseEvent* p_event) override
    {
        PublishingResult returnResult { };
        
        for (auto& callback : _callbacks)
        {
            PublishingResult result = callback(*(const TEvent*)p_event);
            returnResult = (PublishingResult)(returnResult | result);
            
            if (result & PublishingResult::SKIP)
            {
                break;
            }
        }

        return (PublishingResult)returnResult;
    }
    
    template <typename TPublisher>
    constexpr void BindCallback(PublishingResult (TPublisher::*p_func)(const TEvent&), TPublisher* p_publisher)
    {
        BindCallback(std::bind(p_func, p_publisher, std::placeholders::_1));
    }

    constexpr void BindCallback(std::function<PublishingResult(const TEvent&)> p_listenCallback)
    {
        _callbacks.push_back(p_listenCallback);
    }
    
    constexpr void UnbindAllCallbacks()
    {
        _callbacks.clear();
    }
    
    constexpr size_t GetListeningEventType() const override
    { return typeid(TEvent).hash_code(); }

private:
    std::vector<std::function<PublishingResult(const TEvent& p_event)>> _callbacks;
};

class EventBusFactory
{
    EventBusFactory() = default;
    
public:
    ~EventBusFactory()
    {
        
    }
    
    template <class TCreator>
    static EventBus* CreateChildBus()
    {
        
    }

private:
    inline EventBusFactory& GetInstance() const
    {
        static EventBusFactory instance;
        return instance;
    }

private:
    std::vector<EventBus> _eventBuses;
    static uint32_t _idCounter;
};

class EventBus
{
private:
    EventBus(EventBus* p_parent)
    {
        
    }
    
public:
    EventBus(const std::string& p_name = "Main")
    {
        _listeners.reserve(64);
    }
    
    EventBus(const EventBus&) = delete;
    EventBus(EventBus&&) = delete;
    
    template <typename TEvent>
    requires std::is_base_of_v<BaseEvent, TEvent>
    void Publish(const TEvent& p_event, PublishingScope p_scope = PublishingScope::INHERIT)
    {
        TIMER("Publishing \"{}\" event", typeid(TEvent).name());

        PublishingResult result { };

        for (auto listener : _listeners)
        {
            if (listener->GetListeningEventType() != p_event.GetEventTypeID())
            {
                continue;
            }

            PublishingResult listenerResult = listener->Publish(&p_event);
            result = (PublishingResult)(result | listenerResult);
            if (listenerResult & PublishingResult::BLOCK_LOCAL)
            {
                break;
            }
        }

        if (!(result & BLOCK_LOCAL))
        {
            // call sub buses
        }

        if (!(result & BLOCK_GLOBAL))
        {
            // call buses above
        }
    }

    template <typename TEvent>
    requires std::is_base_of_v<BaseEvent, TEvent>
    std::shared_ptr<EventListener<TEvent>> CreateListener()
    {
        auto ptr = std::make_shared<EventListener<TEvent>>([&](BaseEventListener* p_listener) { DestroyListenerHandler(p_listener); }) ;
        _listeners.push_back(ptr.get());
        return ptr;
    }

private:
    void DestroyListenerHandler(BaseEventListener* p_destroyedListener)
    {
        for (uint32_t i = 0; i < _listeners.size(); i++)
        {
            if (_listeners[i] == p_destroyedListener)
            {
                _listeners.erase(_listeners.begin() + i);
                break;
            }
        }
    }

private:
    std::vector<BaseEventListener*> _listeners;
    uint32_t _id;
};

PublishingResult OnTestCallback(const GlobalTestEvent& p_event)
{
    ENGINE_INFO("Listened event {} with value {}", p_event.GetName(), p_event.TestInt);
    return PublishingResult::PASS;
}

PublishingResult BlockTestCallback(const GlobalTestEvent& p_event)
{
    ENGINE_INFO("Blocked event {} with value {}", p_event.GetName(), p_event.TestInt);
    return PublishingResult::BLOCK;
}

class TestClass
{
private:
    PublishingResult OnTestCallback(const GlobalTestEvent&)
    {
        return PublishingResult::PASS;
    }

public:
    void ListenEvent()
    {
        auto mainBus = EventBus();
        auto listener = mainBus.CreateListener<GlobalTestEvent>();
       listener->BindCallback(&TestClass::OnTestCallback, this);
    }
};


int main(int argc, char* argv[])
{
    DeepEngine::Debug::Logger::Initialize("Logs/engine.log");
    auto mainBus = EventBus();
    auto listener = mainBus.CreateListener<GlobalTestEvent>();
    listener->BindCallback(OnTestCallback);
    listener->BindCallback(BlockTestCallback);
    listener->BindCallback(OnTestCallback);


    auto globalEvent = GlobalTestEvent();
    globalEvent.TestInt = 20;
    mainBus.Publish<GlobalTestEvent>(globalEvent);
    
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
