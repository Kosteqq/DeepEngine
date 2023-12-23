#include <stack>

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

enum class EventScope
{
    LOCAL = 0,
    GLOBAL = 1
};

struct BusObject
{
public:
    BusObject(const BusObject& p_other) = delete;
    BusObject(BusObject&& p_other) = delete;
    
    BusObject() = default;
    virtual ~BusObject() = default;
    
    constexpr virtual const char* GetName() const = 0;
    constexpr virtual EventScope GetPublishingScope() const = 0;
    constexpr virtual size_t GetTypeID() const = 0;
};

struct BaseEvent : BusObject
{ };

#define BEGIN_GLOBAL_EVENT_DEFINITION(Name)                             \
    struct Name final : BaseEvent {                                     \
        constexpr const char* GetName() const override                  \
        { return #Name; }                                               \
        constexpr EventScope GetPublishingScope() const override   \
        { return EventScope::GLOBAL; }                             \
        size_t GetTypeID() const override                               \
        { return typeid(Name).hash_code(); }

#define BEGIN_LOCAL_EVENT_DEFINITION(Name)                              \
    struct Name final : BaseEvent {                                     \
        constexpr const char* GetName() const override                  \
        { return #Name; }                                               \
        constexpr EventScope GetPublishingScope() const override   \
        { return EventScope::LOCAL; }                              \
        size_t GetTypeID() const override                               \
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
    PASS = 0,
    BLOCK = 1,
};


template <typename TObject>
class BusListener
{
protected:
    BusListener(std::function<void(BusListener*)>&& p_onDestroyCallback)
        : _onDestroyCallback(p_onDestroyCallback)
    { }

public:
    BusListener(const BusListener&) = delete;
    BusListener(BusListener&&) = delete;
    
    virtual ~BusListener()
    {
        _onDestroyCallback(this);
    }

protected:
    virtual PublishingResult PublishFromListeners(const TObject* p_event) = 0;
    constexpr virtual size_t GetListeningObjectType() const = 0;

private:
    const std::function<void(BusListener*)> _onDestroyCallback;
};

template <typename TObject, typename TListener>
requires std::is_base_of_v<BusObject, TObject>
    && std::is_base_of_v<BusListener<TObject>, TListener>
class Bus
{
protected:
    Bus() = default;
    Bus(Bus<TObject, TListener>* p_parent) : _parentBus(p_parent)
    {
        _listeners.reserve(32);
        _childBuses.reserve(32);
    }

public:
    Bus(const Bus&) = delete;
    Bus(Bus&&) = delete;
    
    virtual ~Bus() = default;

protected:
    template <typename T = TListener, typename ...TArgs>
    requires std::is_base_of_v<TListener, T>
    constexpr std::shared_ptr<T> AddListener(TArgs... p_args)
    {
        auto ptr = std::make_shared<T>(p_args, [&](BusListener<TObject>* p_listener) { DestroyListenerHandler(p_listener); }) ;
        _listeners.push_back(ptr.get());
        return ptr;
    }
    
    template <typename T = TListener>
    requires std::is_base_of_v<TListener, T>
    constexpr std::shared_ptr<T> AddListener()
    {
        auto ptr = std::make_shared<T>([&](BusListener<TObject>* p_listener) { DestroyListenerHandler(p_listener); }) ;
        _listeners.push_back(ptr.get());
        return ptr;
    }
    
    constexpr const std::vector<TListener*>& GetListeners() const
    { return _listeners; }
    
    constexpr Bus<TObject, TListener>* GetParentBus() const
    { return _parentBus; }
    
    constexpr const std::vector<Bus<TObject, TListener>*>& GetChildBuses() const
    { return _childBuses; }

private:
    void DestroyListenerHandler(BusListener<TObject>* p_destroyedListener)
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
    std::vector<TListener*> _listeners;
    Bus<TObject, TListener>* _parentBus = nullptr;
    std::vector<Bus<TObject, TListener>*> _childBuses;
};


class BaseEventListener : public BusListener<BaseEvent>
{
    friend class EventBus;
    
protected:
    BaseEventListener(std::function<void(BusListener<BaseEvent>*)> p_onDestroyCallback)
        : BusListener(std::move(p_onDestroyCallback))
    {
    }
    
public:
    BaseEventListener(const BaseEventListener&) = delete;
    BaseEventListener(BaseEventListener&&) = delete;
};


template <typename TEvent>
requires std::is_base_of_v<BaseEvent, TEvent>
class EventListener final : public BaseEventListener
{
public:
    EventListener(std::function<void(BusListener<BaseEvent>*)> p_onDestroyCallback)
        : BaseEventListener(std::move(p_onDestroyCallback))
    {
        _callbacks.reserve(16);
    }
    
    EventListener(const EventListener&) = delete;
    EventListener(EventListener&&) = delete;

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
    
    constexpr size_t GetListeningObjectType() const override
    { return typeid(TEvent).hash_code(); }

protected:
    PublishingResult PublishFromListeners(const BaseEvent* p_event) override
    {
        for (auto& callback : _callbacks)
        {
            PublishingResult result = callback(*(const TEvent*)p_event);
            
            if (result == PublishingResult::BLOCK)
            {
                return PublishingResult::BLOCK;
            }
        }

        return PublishingResult::PASS;
    }

private:
    std::vector<std::function<PublishingResult(const TEvent& p_event)>> _callbacks;
};


class EventBus : public Bus<BaseEvent, BaseEventListener>
{
private:
    EventBus(EventBus* p_parent) : Bus(p_parent)
    { }
    
public:
    EventBus() : Bus()
    { }
    EventBus(const EventBus&) = delete;
    EventBus(EventBus&&) = delete;

    template <typename TEvent>
    requires std::is_base_of_v<BusObject, TEvent>
    std::shared_ptr<EventListener<TEvent>> CreateListener()
    {
        return AddListener<EventListener<TEvent>>();
    }

    
    template <typename T>
    requires std::is_base_of_v<BaseEvent, T>
    void Publish(const T& p_event)
    {
        struct PublishStack
        {
            EventBus* Bus;
            uint32_t BusChildIndex;
        };
        
        TIMER("Publishing \"{}\" event", p_event.GetName());
        EventBus* rootBus;

        switch (p_event.GetPublishingScope())
        {
        case EventScope::LOCAL:
            rootBus = this;
            break;
        case EventScope::GLOBAL:
            rootBus = GetTopParent(this);
            break;
        default:
            ENGINE_ERR("Unhandled EventScope");
            assert(false);
            return;
        }

        std::stack<PublishStack> stack;
        stack.emplace(rootBus, 0);
        
        while (stack.size() > 0)
        {
            PublishStack& stackElement = stack.top();
            const auto& childs = stackElement.Bus->GetChildBuses();

            // Publish in self listeners before move to first child bus
            if (stackElement.BusChildIndex == 0)
            {
                if (PublishInListeners(stackElement.Bus, p_event) == PublishingResult::BLOCK)
                {
                    break;
                }
            }

            if (stackElement.BusChildIndex >= childs.size())
            {
                stack.pop();
                continue;
            }

            EventBus* nextChild = static_cast<EventBus*>(childs[stackElement.BusChildIndex]);
            stackElement.BusChildIndex++;
            
            stack.emplace(nextChild, 0);
        }
    }

private:
    static PublishingResult PublishInListeners(const EventBus* p_bus, const BaseEvent& p_event)
    {
        for (BaseEventListener* listener : p_bus->GetListeners())
        {
            if (listener->GetListeningObjectType() != p_event.GetTypeID())
            {
                continue;
            }

            if (listener->PublishFromListeners(&p_event) == PublishingResult::BLOCK)
            {
                return PublishingResult::BLOCK;
            }
        }
        
        return PublishingResult::PASS;
    }

    static EventBus* GetTopParent(EventBus* p_bus)
    {
        if (p_bus->GetParentBus() != nullptr)
        {
            return GetTopParent(dynamic_cast<EventBus*>(p_bus->GetParentBus()));
        }

        return p_bus;
    }
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
       //  auto mainBus = EventBus();
       //  auto listener = mainBus.CreateListener<GlobalTestEvent>();
       // listener->BindCallback(&TestClass::OnTestCallback, this);
    }
};

struct DUPA
{
    int Test;
};

struct DUPAA : DUPA
{
    
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
    mainBus.Publish(globalEvent);
    
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
