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

class VulkanObject
{
    friend class VulkanFactory;

    void (*TerminateFunc)(VulkanObject*);

public:
    ~VulkanObject() = default;

    bool IsValid() const
    {
        return _isValid;
    }
    
private:
    bool _isValid;
    uint32_t _factoryID;
    uint32_t _objectID;
    

    std::list<std::weak_ptr<VulkanObject>> _subobjects;
};

template <typename T>
concept VulkanObjectKind = std::is_base_of_v<VulkanObject, T>;

template <typename T>
concept PointerDeleterKind = requires(T t)
{
    { t.operator() };
};

class VulkanFactory
{

public:
    VulkanFactory()
    {
    }

    ~VulkanFactory()
    {
    }

    void Bind()
    {
        _bindInstance = this;
    }

private:
    template <VulkanObjectKind T>
    class VulkanSubFactory
    {
        static void Terminate(T*);
    };

public:
    template <VulkanObjectKind T>
    using FactoryOf = VulkanSubFactory<T>;

    static void TerminateObject(const std::shared_ptr<VulkanObject>& p_object)
    {
        TerminateObject(p_object.get());
    }

private:
    static void TerminateObject(VulkanObject* p_object)
    {
        if (!p_object->IsValid())
        {
            return;
        }
        std::cout << "Terminating Object" << std::endl;

        // revers it lol
        for (auto& subObject : p_object->_subobjects)
        {
            if (!subObject.expired())
            {
                TerminateObject(subObject.lock().get());
            }
        }
        
        p_object->TerminateFunc(p_object);

        p_object->_isValid = false;
    }
    
private:
    static void DestroyPointerHandler(VulkanObject* p_object)
    {
        std::cout << "Destroying Pointer" << std::endl;
        TerminateObject(p_object);
        delete p_object;
    }
    
    template <VulkanObjectKind T>
    std::shared_ptr<T> CreateObject(T* p_instance, void(*TerminateFunc)(VulkanObject*))
    {
        ((VulkanObject*)p_instance)->TerminateFunc = TerminateFunc;
        std::shared_ptr<T> ptr = std::shared_ptr<T>(p_instance, DestroyPointerHandler);
        
        return ptr;
    }
    
    template <VulkanObjectKind T, VulkanObjectKind ParentType>
    std::shared_ptr<T> CreateObject(T* p_instance, void(*TerminateFunc)(VulkanObject*), const std::shared_ptr<ParentType>& p_parentObject)
    {
        ((VulkanObject*)p_instance)->TerminateFunc = TerminateFunc;
        std::shared_ptr<T> ptr = std::shared_ptr<T>(p_instance, DestroyPointerHandler);
        p_parentObject->_subobjects.push_back(std::weak_ptr<T>(ptr));
        return ptr;
    }

    static VulkanFactory GetInstance()
    { return VulkanFactory(); }

private:
    static VulkanFactory* _bindInstance;
};

class Semaphore : public VulkanObject
{
public:
    Semaphore(VkSemaphore p_handler) : _handler(p_handler)
    {
        
    }
    
    VkSemaphore GetVulkanHandler() const
    { return _handler; }

private:
    VkSemaphore _handler;
};

template <>
class VulkanFactory::VulkanSubFactory<Semaphore>
{
public:
    static std::shared_ptr<Semaphore> Create()
    {
        auto factory = GetInstance();

        auto semaphore = new Semaphore(VK_NULL_HANDLE);
        
        auto newObject = factory.CreateObject(semaphore, Terminate);
        return newObject;
    }

private:
    static void Terminate(VulkanObject* p_object)
    {
        std::cout << "Terminateeee" << std::endl;
    }
};



int main(int p_argc, char* p_argv[])
{    
    Debug::Logger::Initialize("Logs/engine.log");
    auto engineEventBus = Core::Events::EventBus();

    Core::Scene::Scene scene;
    {
        auto factory = VulkanFactory();
        auto sempahore = VulkanFactory::FactoryOf<Semaphore>::Create();
        sempahore.reset();
        VulkanFactory::TerminateObject(sempahore);
    }

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