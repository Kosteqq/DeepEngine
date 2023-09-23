#pragma once
#include "Debugs/InitializationMilestone.h"
#include "Debugs/Logger.h"

#include <memory>
#include <typeindex>
#include <type_traits>
#include <vector>

namespace DeepEngine::Core::Architecture
{
    class EngineSubsystemsManager;

    class EngineSubsystem
    {
    protected:
        friend class EngineSubsystemsManager;

        EngineSubsystem(const char* p_subsystemName) :
            _subsystemLogger(Debug::Logger::CreateLoggerInstance(p_subsystemName)),
            _initializeMilestone(Debug::InitializationMilestone::Create(p_subsystemName))
        { }

        virtual bool Init() = 0;
        virtual void Destroy() = 0;
        virtual void Tick() = 0;

    protected:
        EngineSubsystemsManager* _subsystemsManager;
        std::shared_ptr<Debug::Logger> _subsystemLogger;
        Debug::InitializationMilestone _initializeMilestone;
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
        EngineSubsystemsManager();
        ~EngineSubsystemsManager();

        bool Init();
        void Tick();
        
    public:
        template <typename T, class... Args>
        requires std::is_base_of_v<EngineSubsystem, T>
        void CreateSubsystem(Args... p_args)
        {
            auto newSubmodule = reinterpret_cast<EngineSubsystem*>(new T(std::forward<Args>(p_args)...));
            newSubmodule->_subsystemsManager = this;
            _subsystems.push_back((newSubmodule));
            _subsystemsMap[typeid(T)] = newSubmodule;
        }

        template <typename T>
        requires std::is_base_of_v<EngineSubsystem, T>
        const SubsystemPtr<T> GetSubsystem()
        {
            return SubsystemPtr<T>((T*)_subsystemsMap[typeid(T)]);
        }

        template <typename T>
        requires std::is_base_of_v<EngineSubsystem, T>
        const SubsystemPtr<T> GetSubsystem(const std::type_index p_type)
        {
            return SubsystemPtr<T>((T*)_subsystemsMap[p_type]);
        }

    private:
        std::vector<EngineSubsystem*> _subsystems;
        std::unordered_map<std::type_index, EngineSubsystem*> _subsystemsMap;
    };
}

#define TRACE(...) LOG_TRACE(_subsystemLogger, __VA_ARGS__)  
#define DEBUG(...) LOG_DEBUG(_subsystemLogger, __VA_ARGS__)  
#define INFO(...) LOG_INFO(_subsystemLogger, __VA_ARGS__)  
#define WARN(...) LOG_WARN(_subsystemLogger, __VA_ARGS__)  
#define ERR(...) LOG_ERR(_subsystemLogger, __VA_ARGS__)  
