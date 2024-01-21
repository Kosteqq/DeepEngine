#pragma once
#include <iostream>
#include <functional>

#include "Debugs/InitializationMilestone.h"
#include "Debugs/Logger.h"
#include "Debugs/Timing.h"

#include <memory>
#include <typeindex>
#include <type_traits>
#include <vector>

#include "EventBus/EventBus.h"
#include "Scene/Scene.h"


namespace DeepEngine::Architecture
{
    class EngineSubsystemsManager;

    class EngineSubsystem
    {
    public:
        virtual ~EngineSubsystem() = default;

    protected:
        friend class EngineSubsystemsManager;

        EngineSubsystem(EventBus& p_engineEventBus, const char* p_subsystemName) :
            _subsystemLogger(Debug::Logger::CreateLoggerInstance(p_subsystemName)),
            _initializeMilestone(Debug::InitializationMilestone::Create(p_subsystemName)),
            _internalSubsystemEventBus(p_engineEventBus.CreateChildEventBus())
        { }

        virtual bool Init() = 0;
        virtual void Destroy() = 0;
        virtual void Tick(const Scene::Scene& p_scene) = 0;

    protected:
        EngineSubsystemsManager* _subsystemsManager;
        std::shared_ptr<Debug::Logger> _subsystemLogger;
        Debug::InitializationMilestone _initializeMilestone;

        EventBus& _internalSubsystemEventBus;
    };

    class EngineSubsystemsManager
    {
    public:
        template <typename T>
        struct SubsystemPtr
        {
            friend class EngineSubsystemsManager;
            
        private:
            SubsystemPtr(T* p_ptr) : _ptr(p_ptr)
            { }
            
            SubsystemPtr(SubsystemPtr& p_other) = delete;
            SubsystemPtr(SubsystemPtr&& p_other) = delete;

        public:
            T* operator->() const
            { return _ptr; }

        private:
            T* const _ptr;
        };
        
    public:
        EngineSubsystemsManager(EventBus& p_engineEventBus);
        ~EngineSubsystemsManager();

        bool Init();
        void Tick(const Scene::Scene& p_scene);
        
    public:
        template <typename T, class... Args>
        requires std::is_base_of_v<EngineSubsystem, T>
        T* CreateSubsystem(Args... p_args)
        {
            TIMER(fmt::format("Creating submodule: \"{}\"", typeid(T).name()).c_str());
            auto newSubmodule = new T(_engineEventBus, std::forward<Args>(p_args)...);
            _subsystems.push_back(reinterpret_cast<EngineSubsystem*>(newSubmodule));
            return newSubmodule; 
        }

    private:
        std::vector<EngineSubsystem*> _subsystems;
        std::unordered_map<std::type_index, EngineSubsystem*> _subsystemsMap;

        EventBus& _engineEventBus;
    };
}

#define TRACE(...) LOG_TRACE(_subsystemLogger, __VA_ARGS__)  
#define DEBUG(...) LOG_DEBUG(_subsystemLogger, __VA_ARGS__)  
#define INFO(...) LOG_INFO(_subsystemLogger, __VA_ARGS__)  
#define WARN(...) LOG_WARN(_subsystemLogger, __VA_ARGS__)  
#define ERR(...) LOG_ERR(_subsystemLogger, __VA_ARGS__)  
