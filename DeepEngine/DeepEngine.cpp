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

template <class T>
class VulkanObject
{
public:
    T GetVkObject() const
    { return _vkObject; }
    
protected:
    T _vkObject;
};

class VulkanObjectObserver
{
public:
    virtual ~VulkanObjectObserver() = default;
    
    virtual void VulkanObjectOutOfReference(uint32_t p_id) = 0;
};

template <class T, class Y>
requires std::is_base_of_v<VulkanObject<T>, Y>
class VulkanObjectRef
{
private:
    VulkanObjectRef(uint32_t p_refID, VulkanObject<T>& p_objectRef, bool& p_isValid,
        uint32_t& p_refCounter, VulkanObjectObserver& p_observer)
        : _refID(p_refID), _objectRef(p_objectRef), _isValid(p_isValid),
        _refCounter(p_refCounter), _observer(p_observer)
    {
        _refCounter++;
    }
    
public:
    VulkanObjectRef(const VulkanObjectRef& p_base)
        : _refID(p_base._refID), _objectRef(p_base._objectRef), _isValid(p_base._isValid),
        _refCounter(p_base._refCounter), _observer(p_base._observer)
    {
        if (_isValid)
        {
            _refCounter++;
        }
    }

    ~VulkanObjectRef()
    {
        _refCounter--;
        
        if (_refCounter <= 0 && _isValid)
        {
            _observer.VulkanObjectOutOfReference(_refID);
            _isValid = false;
        }
    }

    bool IsValid() const
    { return _isValid; }

    VulkanObject<T>& GetObjectRef() const
    { return _objectRef; }
    
private:
    uint32_t _refID;
    VulkanObject<T>& _objectRef;
    bool& _isValid;
    uint32_t& _refCounter;
    VulkanObjectObserver& _observer;
};

// Builder??
// Data??
// T = Wrapper
template <class T, class Y>
requires std::is_base_of_v<VulkanObject<T>, Y>
class VulkanObjectFactory : VulkanObjectObserver
{
public:
    VulkanObjectFactory() = default;
    ~VulkanObjectFactory() override = default;

    void VulkanObjectOutOfReference(uint32_t p_id) override
    {
        delete _objects[p_id].VulkanObject;
    }

protected:
    struct Object
    {
        VulkanObject<T>* VulkanObject;
        uint32_t RefCounter;
        bool IsValid;
    };

    VulkanObjectRef<T, Y> CreateRef(const Object& p_object)
    {
        _objectIdCounter++;
        _objects[_objectIdCounter] = p_object;
        return VulkanObjectRef<T, Y>(
            _objectIdCounter,
            *p_object.VulkanObject,
            p_object.IsValid,
            p_object.RefCounter,
            *this);
    }

private:
    uint32_t _objectIdCounter;
    std::unordered_map<uint32_t, Object> _objects;
};



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